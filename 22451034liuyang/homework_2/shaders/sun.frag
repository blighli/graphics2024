#version 330 core
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 emissiveColor;

void main()
{

    vec3 textureColor=texture(ourTexture,texCoord).rgb;
    vec3 result = textureColor+emissiveColor;
    FragColor = vec4(result, 1.0f);
}
