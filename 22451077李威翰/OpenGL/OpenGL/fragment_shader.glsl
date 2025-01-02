#version 420 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int outID;
in VS_OUT
{
    vec2 vUV;
} fs_in;

uniform sampler2D sphereTex;
uniform int objID;
void main()
{
    vec4 color = texture(sphereTex, fs_in.vUV);
    FragColor = vec4(pow(color.rgb, vec3(1.0 / 2.2)), 1.0);
    outID = objID;
}