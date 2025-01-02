#version 420 core

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int outID;
in VS_OUT
{
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vUV;
} fs_in;

uniform sampler2D tex;
uniform sampler2D sunTex;

uniform vec3 sunPos;
uniform float sunRadius;
uniform vec3 camearPos;
uniform int objID;

float getTouchPoint(vec3 dir)
{

    vec3 L = sunPos - fs_in.vFragPos;
    float tca = dot(L, dir);
    float d2 = dot(L, L) - tca * tca;
    if(d2 > sunRadius * sunRadius) return -1.0;
    float thc = sqrt(sunRadius * sunRadius - d2);
    float t0 = tca - thc;
    float t1 = tca + thc;
    if(t0 < 0) t0 = t1;
    if(t0 < 0) return -1.0;
    return t0;
}

void main()
{
    vec3 viewDir = normalize(camearPos - fs_in.vFragPos);

    vec3 reflectDir = reflect(-viewDir, fs_in.vNormal);

    float t = getTouchPoint(reflectDir);

    vec3 albedo = vec3(0.0);
    if(t > 0.0)
    {
        vec3 touch = t * reflectDir;
        vec3 sunLocalDir = normalize(touch - sunPos);
        float theta = acos(sunLocalDir.y);
        float phi = acos(sunLocalDir.x / (sin(theta) + 0.0001));
        albedo = texture(sunTex, vec2(theta, phi)).rgb;
    }
    vec3 diffuseColor = albedo * texture(tex, fs_in.vUV).rgb * max(dot(fs_in.vNormal, reflectDir), 0.0);
    vec3 color = diffuseColor;
    FragColor = vec4(pow(color, vec3(1.0 / 2.2)), 1.0);
    outID = objID;
}