#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <math.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glut.h>

#include "Sphere.h"

// 视口的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// 输入控制
void processInput(GLFWwindow* window);

// 设置
const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 600;

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


// 光照
glm::vec3 lightPos(0.6f, 0.5f, 1.0f);

void MySphere(float r = 0.5f, float* sphere_vertices = nullptr, float* sphere_normals = nullptr, float* sphere_texCoords = nullptr, int* sphere_indices = nullptr, int nSlices = 50, int nStacks = 50) {
    int nVerts = (nStacks + 1) * (nSlices + 1);
    float theta, phi;
    float thetaFac = 3.14f * 2.0f / nSlices;
    float phiFac = 3.14f * 1.0f / nStacks;
    float nx, ny, nz;
    int idx = 0;

    // 计算球体的顶点，法线和纹理坐标
    for (int i = 0; i <= nSlices; i++) {
        theta = i * thetaFac;
        for (int j = 0; j <= nStacks; j++) {
            phi = j * phiFac;

            // 计算顶点位置
            nx = sinf(phi) * cosf(theta);
            ny = sinf(phi) * sinf(theta);
            nz = cosf(phi);
            sphere_vertices[idx * 3] = r * nx;
            sphere_vertices[idx * 3 + 1] = r * ny;
            sphere_vertices[idx * 3 + 2] = r * nz;

            // 计算法线（单位向量，指向球心）
            sphere_normals[idx * 3] = nx;
            sphere_normals[idx * 3 + 1] = ny;
            sphere_normals[idx * 3 + 2] = nz;

            // 计算纹理坐标
            // s = theta / (2 * pi), t = phi / pi
            sphere_texCoords[idx * 2] = (float)i / 50; // [0, 1]
            sphere_texCoords[idx * 2 + 1] = (float)j / 50;        // [0, 1]

            idx++;
        }
    }

    // 定义球体坐标索引
    int indx = 0;
    for (int i = 0; i < nStacks; i++) {
        for (int j = 0; j < nSlices; j++) {
            int i0 = i * (nSlices + 1) + j;
            int i1 = i0 + 1;
            int i2 = i0 + (nSlices + 1);
            int i3 = i2 + 1;
            if ((j + i) % 2 == 1) {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i1;
                sphere_indices[indx + 3] = i1;
                sphere_indices[indx + 4] = i2;
                sphere_indices[indx + 5] = i3;
                indx += 6;
            }
            else {
                sphere_indices[indx] = i0;
                sphere_indices[indx + 1] = i2;
                sphere_indices[indx + 2] = i3;
                sphere_indices[indx + 3] = i0;
                sphere_indices[indx + 4] = i3;
                sphere_indices[indx + 5] = i1;
                indx += 6;
            }
        }
    }
}

GLuint loadTexture(const char* path)
{
    GLuint textureID;
    glGenTextures(1, &textureID);  // 生成一个纹理ID
    glBindTexture(GL_TEXTURE_2D, textureID);  // 绑定纹理

    // 设置纹理的过滤方式和包裹方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);  // 水平方向的包裹方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);  // 垂直方向的包裹方式

    // 加载图像
    int width, height, nrChannels;
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        // 生成纹理
        if (nrChannels == 3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else if (nrChannels == 4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glGenerateMipmap(GL_TEXTURE_2D);  // 生成纹理的 mipmaps

        // 释放图像数据
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}


int main()
{
    // 实例化GLFW窗口
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口对象
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "lexiaoyuan", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 定义球体坐标
    float r = 0.5;
    int nSlices = 50; // 经线
    int nStacks = 50; // 纬线
    int nVerts = (nStacks + 1) * (nSlices + 1);
    int elements = nSlices * nStacks * 6;
    const int space = nVerts * 3;
    float sphere_vertices[51 * 51 * 3];
    float sphere_normals[51 * 51 * 3];
    float sphere_texcoord[51 * 51 * 2];
    int sphere_indices[50 * 50 * 6];
    
    MySphere(0.5f, sphere_vertices, sphere_normals, sphere_texcoord, sphere_indices ,nSlices, nStacks);


    float interleavedData[51 * 51 * (3 + 3 + 2)]; // 假设每个顶点有 3 个位置，3 个法线，2 个纹理坐标

    for (int i = 0; i < nVerts; i++) {
        interleavedData[i * 8] = sphere_vertices[i * 3];        // 顶点位置 x
        interleavedData[i * 8 + 1] = sphere_vertices[i * 3 + 1]; // 顶点位置 y
        interleavedData[i * 8 + 2] = sphere_vertices[i * 3 + 2]; // 顶点位置 z

        interleavedData[i * 8 + 3] = sphere_normals[i * 3];     // 法线 x
        interleavedData[i * 8 + 4] = sphere_normals[i * 3 + 1]; // 法线 y
        interleavedData[i * 8 + 5] = sphere_normals[i * 3 + 2]; // 法线 z

        interleavedData[i * 8 + 6] = sphere_texcoord[i * 2];   // 纹理坐标 s
        interleavedData[i * 8 + 7] = sphere_texcoord[i * 2 + 1]; // 纹理坐标 t
    }

    // 定义顶点缓冲对象
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // 定义顶点数组对象
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // 定义索引缓冲对象
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // 绑定顶点缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 复制顶点数据到缓冲内存
    //glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, sizeof(interleavedData), interleavedData, GL_STATIC_DRAW);

    // 绑定顶点数组对象
    glBindVertexArray(VAO);

    // 绑定索引缓冲对象
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_indices), sphere_indices, GL_STATIC_DRAW);


    // 创建顶点着色器对象
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // 附加顶点着色器代码到顶点着色器对象
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    // 编译顶点着色器
    glCompileShader(vertexShader);

    // 创建片段着色器对象
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 附加片段着色器代码到片段着色器对象
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // 编译片段着色器
    glCompileShader(fragmentShader);

    // 创建一个着色器程序对象
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    // 把着色器对象附加到程序上
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    // 链接着色器程序
    glLinkProgram(shaderProgram);

    // 着色器对象链接成功后可以删掉了
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // 位置属性
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    //glEnableVertexAttribArray(1);
    // 顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 顶点法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 顶点纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint textureID = loadTexture("1.jpg");
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    GLuint textureLoc = glGetUniformLocation(shaderProgram, "texture1");
    glUniform1i(textureLoc, 0);  // 将纹理单元 0 绑定到纹理


    // 定义视口
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // 注册回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 渲染循环
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活着色器程序对象
        glUseProgram(shaderProgram);

        // 绑定顶点数组
        glBindVertexArray(VAO);

        // 绑定索引缓冲对象
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

        // 使用线框模式
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // 使用填充模式（默认）
        // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        /*lightPos.x = 1.0f + sin(glfwGetTime()) * 2.0f;
        lightPos.y = sin(glfwGetTime() / 2.0f) * 1.0f;*/

        glUniform3f(glGetUniformLocation(shaderProgram, "objectColor"), 1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(shaderProgram, "lightColor"), 1.0f, 1.0f, 1.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &glm::vec3(0.0f, 0.0f, 3.0f)[0]);

        // 定义模型矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        // 定义观察矩阵
        glm::mat4 view = glm::mat4(1.0f);
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

        // 定义投影矩阵
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // 检索矩阵的统一位置
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        // 将它们传递给着色器
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, &projection[0][0]);


        // 绘制
        glDrawElements(GL_TRIANGLES, elements, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, elements);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // 释放之前分配的资源
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
