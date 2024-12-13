<template>
  <div>
    <canvas ref="canvasRef" width="400" height="400"></canvas>
  </div>
</template>

<script setup lang="ts">
import { onMounted, ref } from 'vue'

const canvasRef = ref<HTMLCanvasElement | null>(null)

// 顶点着色器代码
const vertexShaderSource = `
  attribute vec4 a_Position;
  void main() {
    gl_Position = a_Position;
    gl_PointSize = 20.0;
  }
`

// 片元着色器代码
const fragmentShaderSource = `
  void main() {
    gl_FragColor = vec4(0, 255, 255, 1);
  }
`

const initShaders = (gl: WebGLRenderingContext, vertexShaderSource: string, fragmentShaderSource: string) => {
  // 创建顶点着色器
  const vertexShader = gl.createShader(gl.VERTEX_SHADER)
  if (!vertexShader) throw new Error('创建顶点着色器失败')
  
  // 创建片元着色器
  const fragmentShader = gl.createShader(gl.FRAGMENT_SHADER)
  if (!fragmentShader) throw new Error('创建片元着色器失败')

  // 设置着色器源码并编译
  gl.shaderSource(vertexShader, vertexShaderSource)
  gl.shaderSource(fragmentShader, fragmentShaderSource)
  gl.compileShader(vertexShader)
  gl.compileShader(fragmentShader)

  // 创建程序对象
  const program = gl.createProgram()
  if (!program) throw new Error('创建程序对象失败')

  // 添加着色器到程序对象
  gl.attachShader(program, vertexShader)
  gl.attachShader(program, fragmentShader)

  // 链接程序对象
  gl.linkProgram(program)
  gl.useProgram(program)

  return program
}

onMounted(() => {
  const canvas = canvasRef.value
  if (!canvas) return
  
  const gl = canvas.getContext('webgl')
  if (!gl) {
    console.error('WebGL不可用')
    return
  }

  // 初始化着色器
  const program = initShaders(gl, vertexShaderSource, fragmentShaderSource)
  
  // 获取顶点位置变量
  const a_Position = gl.getAttribLocation(program, 'a_Position')
  if (a_Position < 0) {
    console.error('获取attribute变量失败')
    return
  }

  // 设置清除颜色为黑色
  gl.clearColor(0.0, 0.0, 0.0, 1.0)
  // 清除颜色缓冲区
  gl.clear(gl.COLOR_BUFFER_BIT)

  // 设置顶点位置
  gl.vertexAttrib3f(a_Position, 0.0, 0.0, 0.0)
  
  // 绘制一个点
  gl.drawArrays(gl.POINTS, 0, 1)
})
</script>

<style scoped>
canvas {
  border: 1px solid #ccc;
}
</style>

