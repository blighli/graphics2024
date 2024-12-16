#version 330 core
in vec3 fragNormal;
in vec3 fragPosition;
in vec2 texCoord;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform sampler2D ourTexture;

void main()
{
    vec3 textureColor=texture(ourTexture,texCoord).rgb;
    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 viewDir = normalize(viewPos - fragPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 256.0);

    vec3 ambient = 0.04 * lightColor;
    vec3 diffuse = diff * lightColor;
    vec3 specular = spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * textureColor;

    FragColor = vec4(result, 1.0f);
}
