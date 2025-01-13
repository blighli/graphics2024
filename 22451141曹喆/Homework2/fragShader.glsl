#version 460 core

in vec2 tc;
out vec4 color;

//uniform mat4 mv_matrix;
//uniform mat4 proj_matrix;
layout(binding = 0) uniform sampler2D samp;

void main()
{
	color = texture(samp, tc);	
}
