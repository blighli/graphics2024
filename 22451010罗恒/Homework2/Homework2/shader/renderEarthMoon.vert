#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 perspective;
};

out VS_OUT
{
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vUV;
} vs_out;

uniform mat4 model;
uniform mat3 TangentToWorld;

void main()
{
    vec4 paspos = model * vec4(aPos, 1.0);
    vs_out.vFragPos = (paspos / paspos.w).xyz;
    vs_out.vNormal = aNormal;
    vs_out.vUV = aUV;

    gl_Position = perspective * view * model * vec4(aPos, 1.0);
    
}