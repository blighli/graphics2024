#version 420 core
layout(location = 0) out vec4 FragColor;

in VS_OUT
{
    vec3 vPos;
    vec2 vTexCoord;
    mat3 TBN;
    vec3 vNormal;
} fs_in;

struct Texture
{
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D ambient0;
    sampler2D normal0;
    float ns;
};

struct Light
{
    vec3 position;
    vec3 color;
};

uniform Texture uTextures;
uniform vec3 camPos;
uniform Light lights[32];
uniform int lightCount;

vec3 calculateDiffuse(vec3 viewDir, vec3 diffuseAlbedo, vec3 lightDir, vec3 normal, vec3 lightColor)
{
    vec3 color = max(dot(normal, lightDir), 0.0) * lightColor * diffuseAlbedo;
    return color;
}

vec3 calculateSpecular(vec3 viewDir, vec3 specularAlbedo, vec3 lightDir, vec3 normal, vec3 lightColor, float ns)
{
    if(ns == 0)
    {
        ns = 64.0;
    }
    vec3 h = normalize(viewDir + lightDir);
    vec3 color = pow(max(dot(h, normal), 0.0), ns) * specularAlbedo * lightColor;
    return color;
}

void main()
{
    vec3 viewDir = normalize(camPos - fs_in.vPos);
    vec3 shaderColor = vec3(0.0);

    vec3 diffuseAlbedo = texture(uTextures.diffuse0 ,fs_in.vTexCoord).rgb;
    vec3 specularAlbedo = texture(uTextures.specular0, fs_in.vTexCoord).rgb;
    vec3 ambientAlbedo = texture(uTextures.ambient0, fs_in.vTexCoord).rgb;

    vec3 normal = normalize(texture(uTextures.normal0, fs_in.vTexCoord).rgb);
    normal = normalize(normal * 2.0 - 1.0);
    normal = normalize(fs_in.TBN * normal);
    for(int i = 0; i < lightCount; i++)
    {
        float distance = length(lights[i].position - fs_in.vPos);
        vec3 lightDir = normalize(lights[i].position - fs_in.vPos);
        shaderColor += calculateDiffuse(viewDir, diffuseAlbedo, lightDir, normal, lights[i].color) * (1.0 / (distance));
        shaderColor += calculateSpecular(viewDir, specularAlbedo, lightDir, normal, lights[i].color, uTextures.ns) * (1.0 / (distance));
    }

    shaderColor /= (shaderColor + vec3(1.0));
    //shaderColor = (normal * 0.5) + vec3(0.5);
    shaderColor = pow(shaderColor, vec3(1.0 / 2.2));
    FragColor = vec4(shaderColor, 1.0);
}