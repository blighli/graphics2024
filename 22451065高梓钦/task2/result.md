![image-20250109133044730](C:\Users\Adminis\AppData\Roaming\Typora\typora-user-images\image-20250109133044730.png)

使用vertex shader和fragment shader实现光照和地球模型。phong光照模型如下：

```
// phong光照模型
// 定义顶点着色器
const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"            // 顶点位置
"layout (location = 1) in vec3 aNormal;\n"          // 顶点法线
"layout (location = 2) in vec3 aTexCoord;\n"        // 纹理坐标
"out vec3 FragPos;\n"                               // 输出片段位置
"out vec3 Normal;\n"                               // 输出法线
"out vec3 TexCoord;\n"                             // 输出纹理坐标
"uniform mat4 model;\n"                            // 模型矩阵
"uniform mat4 view;\n"                             // 视图矩阵
"uniform mat4 projection;\n"                       // 投影矩阵
"void main()\n"
"{\n"
"   // 计算世界空间中的顶点位置\n"
"   FragPos = vec3(model * vec4(aPos, 1.0));\n"
"   // 计算法线并变换到世界空间\n"
"   Normal = mat3(transpose(inverse(model))) * aNormal;\n"
"   // 计算裁剪空间中的顶点位置\n"
"   gl_Position = projection * view * vec4(FragPos, 1.0);\n"
"   // 传递纹理坐标到片段着色器\n"
"   TexCoord = aTexCoord;\n"
"}\0";

// 定义片段着色器
const char* fragmentShaderSource = "#version 460 core\n"
"out vec4 FragColor;\n"
"uniform sampler2D texture1;\n"  // 纹理采样器，绑定到纹理单元
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"in vec3 TexCoord;\n"  // 传入的纹理坐标
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 objectColor;\n"
"void main()\n"
"{\n"
"   // 环境光照\n"
"   float ambientStrength = 0.1;\n"
"   vec3 ambient = ambientStrength * lightColor;\n"
"   // 漫反射光照\n"
"   vec3 norm = normalize(Normal);\n"
"   vec3 lightDir = normalize(lightPos - FragPos);\n"
"   float diff = max(dot(norm, lightDir), 0.0);\n"
"   vec3 diffuse = diff * lightColor;\n"
"   // 镜面光照\n"
"   float specularStrength = 0.5;\n"
"   vec3 viewDir = normalize(viewPos - FragPos);\n"
"   vec3 reflectDir = reflect(-lightDir, norm);\n"
"   float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);\n"
"   vec3 specular = specularStrength * spec * lightColor;\n"
"   // 纹理颜色\n"
"   vec3 textureColor = texture(texture1, TexCoord.xy).rgb;\n"  // 采样纹理，TexCoord.xy 是纹理坐标
"   // 结果\n"
"   vec3 result = (ambient + diffuse + specular) * objectColor;\n"
"   // 使用纹理颜色混合：可以直接覆盖或进行加权混合\n"
"   FragColor = vec4(result * textureColor, 1.0);\n"  // 将计算的光照结果与纹理颜色相乘，得到最终颜色
"}\0";
```

