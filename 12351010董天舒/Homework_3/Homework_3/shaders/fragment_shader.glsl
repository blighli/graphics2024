// fragment_shader.glsl
#version 330 core
struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

#define NR_LIGHTS 4

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 viewPos;
uniform Light lights[NR_LIGHTS];
uniform sampler2D texture_diffuse1;

void main()
{
    vec3 ambient = vec3(0.0);
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
  
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
  
    for(int i = 0; i < NR_LIGHTS; i++)
    {
        // Ambient
        ambient += lights[i].ambient;
      
        // Diffuse
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += lights[i].diffuse * diff;
      
        // Specular
        float specularStrength = 0.5;
        vec3 reflectDir = reflect(-lightDir, norm);  
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular += lights[i].specular * spec * specularStrength;
    }
  
    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 result = (ambient + diffuse + specular) * textureColor;
    FragColor = vec4(result, 1.0);
}
