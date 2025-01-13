import vert from './shaders/basic.instanced.vert.wgsl?raw'
import frag from './shaders/position.frag.wgsl?raw'
import positionCompute from './shaders/compute.position.wgsl?raw'
import * as box from './util/box'
import { getModelViewMatrix, getProjectionMatrix } from './util/math'

async function initWebGPU() {
    const adapter=await navigator.gpu.requestAdapter()
    if(!adapter) throw new Error('no adapter found')
    const device =await adapter.requestDevice({
        requiredLimits: {
            maxStorageBufferBindingSize: adapter.limits.maxStorageBufferBindingSize
        }
    })
    const canvas=document.querySelector('canvas')
    if(!canvas) throw new Error('no canvas')
    const context=canvas.getContext('webgpu');
    if(!context) throw new Error('no context')
    const format=navigator.gpu.getPreferredCanvasFormat()
    const devicePixelRatio=window.devicePixelRatio||1
    canvas.width=canvas.clientWidth*devicePixelRatio
    canvas.height=canvas.clientHeight*devicePixelRatio
    const size={width:canvas.width,height:canvas.height}
    context.configure({
        device,
        format,
        alphaMode:'opaque'
    })
    return {adapter,device,format,context,size}
}

async function initPipeline(device:GPUDevice,format:GPUTextureFormat,size:{width: number,height: number}) {
    const renderPipeline= await device.createRenderPipelineAsync({
        layout:'auto',
        vertex:{
            module:device.createShaderModule({
                code:vert
            }),
            entryPoint:'main',
            buffers:[{//position
                arrayStride:4*8,
                attributes:[{
                    shaderLocation:0,
                    offset:0,
                    format:'float32x3'
                },{//normal
                    shaderLocation:1,
                    offset:4*3,
                    format:'float32x3'
                },{//uv
                    shaderLocation:2,
                    offset:4*6,
                    format:'float32x2'
                }]
            }]
        },
        primitive:{
            topology:'triangle-list'
        },
        fragment:{
            module:device.createShaderModule({
                code:frag
            }),
            entryPoint:'main',
            targets:[{format}]
        },
        depthStencil:{
            depthWriteEnabled:true,
            depthCompare:'less',
            format:'depth24plus'
        }
    })

    const depthTexture=device.createTexture({
        size,format:'depth24plus',
        usage:GPUTextureUsage.RENDER_ATTACHMENT
    })
    //compute pipeline
    const computePipeline=await device.createComputePipelineAsync({
        layout:'auto',
        compute:{
            module:device.createShaderModule({
                code:positionCompute
            }),
            entryPoint:'main'
        }
    })
    //vertexBuffer
    const vertexBuffer={
        vertex:device.createBuffer({
            size:box.vertex.byteLength,
            usage:GPUBufferUsage.VERTEX|GPUBufferUsage.COPY_DST
        }),
        index:device.createBuffer({
            size:box.index.byteLength,
            usage:GPUBufferUsage.INDEX|GPUBufferUsage.COPY_DST
        })
    }
    device.queue.writeBuffer(vertexBuffer.vertex,0,box.vertex)
    device.queue.writeBuffer(vertexBuffer.index,0,box.index)
    
    //Buffer
    const modelBuffer=device.createBuffer({
        size:4*4*4*MAX,
        usage:GPUBufferUsage.STORAGE|GPUBufferUsage.COPY_DST
    })
    const projectionBuffer=device.createBuffer({
        size:4*4*4,
        usage:GPUBufferUsage.UNIFORM|GPUBufferUsage.COPY_DST
    })
    const mvpBuffer=device.createBuffer({
        size:4*4*4*MAX,
        usage:GPUBufferUsage.STORAGE|GPUBufferUsage.COPY_DST
    })
    const velocityBuffer=device.createBuffer({
        size:4*4*MAX,
        usage:GPUBufferUsage.STORAGE|GPUBufferUsage.COPY_DST
    })
    const inputBuffer=device.createBuffer({
        size:4*7,
        usage:GPUBufferUsage.STORAGE|GPUBufferUsage.COPY_DST
    })

    //resource binding
    const renderGroup=device.createBindGroup({
        layout:renderPipeline.getBindGroupLayout(0),
        entries:[{
            binding:0,
            resource:{
                buffer:mvpBuffer
            }
        }
        ]
    })
    const computeGroup=device.createBindGroup({
        layout:computePipeline.getBindGroupLayout(0),
            entries:[{
                binding:0,
                resource:{
                    buffer:inputBuffer
                }
            },{
                binding:1,
                resource:{
                    buffer:velocityBuffer
                }
            },{
                binding:2,
                resource:{
                    buffer:modelBuffer
                }
            },{
                binding:3,
                resource:{
                    buffer:projectionBuffer
                }
            },{
                binding:4,
                resource:{
                    buffer:mvpBuffer
                }
            }
            ]
    })

    
    return {renderPipeline,computePipeline,vertexBuffer,
        modelBuffer,projectionBuffer,mvpBuffer,velocityBuffer,inputBuffer,
        renderGroup,computeGroup,depthTexture} 
}

function draw(device:GPUDevice,renderPipeline:GPURenderPipeline,computePipeline:GPUComputePipeline,context:GPUCanvasContext,
    vertexBuffer:{vertex:GPUBuffer,index:GPUBuffer},renderGroup:GPUBindGroup,computeGroup:GPUBindGroup,depthTexture:GPUTexture){
    const encoder=device.createCommandEncoder()
    //compute
    const commandPass=encoder.beginComputePass()
    commandPass.setPipeline(computePipeline)
    commandPass.setBindGroup(0,computeGroup)
    commandPass.dispatchWorkgroups(Math.ceil(NUM/128))
    commandPass.end()
    //render
    const renderPass=encoder.beginRenderPass({
        colorAttachments:[{
            view:context.getCurrentTexture().createView(),
            loadOp:'clear',
            clearValue:{r:0,g:0,b:0,a:1},
            storeOp:'store'
        }],
        depthStencilAttachment:{
            view:depthTexture.createView(),
            depthClearValue:1.0,
            depthLoadOp:'clear',
            depthStoreOp:'store'
        }
    })
    renderPass.setPipeline(renderPipeline)
    renderPass.setBindGroup(0,renderGroup)
    renderPass.setVertexBuffer(0,vertexBuffer.vertex)
    renderPass.setIndexBuffer(vertexBuffer.index,'uint16')
    renderPass.drawIndexed(box.indexCount,NUM)
    renderPass.end() 
    device.queue.submit([encoder.finish()])
}

// total objects
let NUM = 150000, MAX = 300000
async function run() {
    const {device,format,context,size}=await initWebGPU()
    const {renderPipeline,computePipeline,vertexBuffer,modelBuffer,projectionBuffer,velocityBuffer,inputBuffer,
        renderGroup,computeGroup,depthTexture} =await initPipeline(device,format,size)
    
    // create data
    const inputArray = new Float32Array([NUM, -500, 500, -250, 250, -500, 500]) // count, xmin/max, ymin/max, zmin/max
    const modelArray = new Float32Array(MAX * 4 * 4)
    const velocityArray = new Float32Array(MAX * 4)
    //
    for(let i=0;i<NUM;i++){
        //mv
        const position={x:Math.random() * 1000 - 500, y: Math.random() * 500 - 250, z:  Math.random() * 1000 - 500}
        const rotation={x:0,y:0,z:0}
        const scale={x:2,y:2,z:2}
        modelArray.set(getModelViewMatrix(position,rotation,scale),i*4*4)

        velocityArray[i * 4 + 0] = Math.random() - 0.5 // x
        velocityArray[i * 4 + 1] = Math.random() - 0.5 // y
        velocityArray[i * 4 + 2] = Math.random() - 0.5 // z
        velocityArray[i * 4 + 3] = 1 // w
    }
    device.queue.writeBuffer(modelBuffer,0,modelArray)
    device.queue.writeBuffer(velocityBuffer,0,velocityArray)
    device.queue.writeBuffer(inputBuffer,0,inputArray)
    
    function frame(){
        const time = performance.now() / 5000
        device.queue.writeBuffer(projectionBuffer,0,getProjectionMatrix(
            size.width/size.height,60 / 180 *Math.PI,0.1,10000,{x:1000 * Math.sin(time), y: 50, z: 1000 * Math.cos(time)}))
            
        draw(device,renderPipeline,computePipeline,context,vertexBuffer,renderGroup,computeGroup,depthTexture)
        requestAnimationFrame(frame)
    }
    frame()

    const range = document.querySelector('input') as HTMLInputElement
    range.max = MAX.toString()
    range.value = NUM.toString()
    range.addEventListener('input', (e:Event)=>{
        NUM = +(e.target as HTMLInputElement).value
        const span = document.querySelector('#num') as HTMLSpanElement
        span.innerHTML = NUM.toString()
        inputArray[0] = NUM
        device.queue.writeBuffer(inputBuffer, 0, inputArray)
    })
}

run()