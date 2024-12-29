#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 projection;
};
out VS_OUT
{
    vec2 vUV;
} vs_out;

uniform mat4 model;
void main()
{
    vs_out.vUV = aUV;

    gl_Position = projection * view * model * vec4(aPos, 1.0);
}