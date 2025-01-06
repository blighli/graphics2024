#version 460 core

in vec3 fragPos;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 fragColor;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform sampler2D textureSampler;
uniform vec3 viewPos;

void main()
{
    vec3 ambient = vec3(0.1); // 环境光
    vec3 result = vec3(0.0);

    vec3 norm = normalize(fragNormal);
    vec3 viewDir = normalize(viewPos - fragPos);

    for (int i = 0; i < numLights; i++) {
        // 漫反射光照
        vec3 lightDir = normalize(lights[i].position - fragPos);
        float diff = max(dot(norm, lightDir), 0.0);

        // 镜面光照
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

        // 组合光照结果
        vec3 lightColor = lights[i].color * lights[i].intensity;
        result += (diff + spec) * lightColor;
    }

    vec3 texColor = texture(textureSampler, fragTexCoord).rgb;
    fragColor = vec4((ambient + result) * texColor, 1.0);
}
