#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    fragNormal = mat3(transpose(inverse(model))) * aNormal;
    fragPosition = vec3(model*vec4(aPos, 1.0));
    texCoord = aTexCoord;
    gl_Position = projection*view*model*vec4(aPos, 1.0f);
}
