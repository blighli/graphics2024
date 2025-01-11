#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 viewPos;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
    int type; // 0: 点光源, 1: 方向光源
};

uniform Material material;
uniform Light light[5];

vec3 CalculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir);

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result = vec3(0.0);

    for (int i = 0; i < 5; i++) {
        if (light[i].type == 0) {
            result += CalculatePointLight(light[i], norm, FragPos, viewDir);
        } else {
            result += CalculateDirectionalLight(light[i], norm, viewDir);
        }
    }

    FragColor = vec4(result, 1.0);
}

vec3 CalculatePointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 ambient = light.ambient * material.ambient;

    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ambient + diffuse + specular;
}

vec3 CalculateDirectionalLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * material.diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * material.specular;

    vec3 ambient = light.ambient * material.ambient;

    return ambient + diffuse + specular;
}