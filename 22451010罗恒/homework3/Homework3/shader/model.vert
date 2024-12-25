#version 420 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec3 aTangent;
layout(location = 3) in vec2 aTexCoord;
layout(std140, binding = 2) uniform PMatrix
{
    mat4 view;
    mat4 projection;
};
out VS_OUT
{
    vec3 vPos;
    vec2 vTexCoord;
    mat3 TBN;
    vec3 vNormal;
} vs_out;

uniform mat4 model;
void main()
{
    vec3 tangent = normalize(transpose(inverse(mat3(model))) * aTangent);
    vec3 normal = normalize(transpose(inverse(mat3(model))) * aNormal);
    tangent = normalize(tangent - dot(normal, tangent) * normal);

    vec3 bitangent = normalize(cross(normal, tangent));
    vs_out.TBN = mat3(tangent, bitangent, normal);

    vec4 pasPos = model * vec4(aPos, 1.0);
    vs_out.vPos = (pasPos / pasPos.w).xyz;
    vs_out.vTexCoord = aTexCoord;
    vs_out.vNormal = aNormal;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}