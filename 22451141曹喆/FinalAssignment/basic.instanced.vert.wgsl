@group(0) @binding(0) var<storage> mvp:array<mat4x4<f32>>;

struct VertexOutput{
    @builtin(position) Pos:vec4<f32>,
    @location(0) fragUV:vec2<f32>,
    @location(1) fragPosition:vec4<f32>
}
@vertex
fn main(
    @builtin(instance_index) index:u32,
    @location(0) position:vec4<f32>,
    @location(1) uv:vec2<f32>
)->VertexOutput{
    //vec4:vec3 automatic replenishment 1
    var output:VertexOutput;
    output.Pos=mvp[index]*position;
    output.fragUV=uv;
    output.fragPosition=(position+vec4(1.0,1.0,1.0,1.0))*0.5;
    
    return output;
}
