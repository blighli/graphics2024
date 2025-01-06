#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 normal;

out vec3 fragPos;
out vec2 fragTexCoord;
out vec3 fragNormal;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

void main()
{
    fragPos = vec3(mv_matrix * vec4(position, 1.0));
    fragNormal = mat3(transpose(inverse(mv_matrix))) * normal; // 转换法线到相机空间
    fragTexCoord = texCoord;
    
    gl_Position = proj_matrix * vec4(fragPos, 1.0);
}
