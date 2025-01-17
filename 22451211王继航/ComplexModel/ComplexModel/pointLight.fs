#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D light_texture;
uniform bool usePointLight;

void main()
{
    if (usePointLight) {
        FragColor = texture(light_texture, TexCoords);
    } else {
        FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
}