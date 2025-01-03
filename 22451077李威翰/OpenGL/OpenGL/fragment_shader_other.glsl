#version 330 core

in vec3 fragPos;
in vec3 normal;
in vec2 texCoords;

out vec4 FragColor;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Light lights[3];
uniform vec3 viewPos;
uniform sampler2D texture_diffuse1;

void main() {
    vec3 result = texture(texture_diffuse1, texCoords).rgb;

    for (int i = 0; i < 3; i++) {
        vec3 lightDir = normalize(lights[i].position - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewPos - fragPos, reflectDir), 0.0), 32);

        vec3 ambient = 0.1 * lights[i].color;
        vec3 diffuse = diff * lights[i].color;
        vec3 specular = spec * lights[i].color;

        result += lights[i].intensity * (ambient + diffuse + specular);
    }

    FragColor = vec4(result, 1.0);
}
