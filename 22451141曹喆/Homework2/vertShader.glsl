#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

layout(binding = 0) uniform sampler2D samp;

out vec2 tc;

void main()
{
	gl_Position = proj_matrix * mv_matrix * vec4(position, 1.f);
	tc = texCoords;
}

