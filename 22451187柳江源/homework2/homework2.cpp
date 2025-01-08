#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

// 球体的顶点着色器（用于地球和月亮）
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"layout (location = 2) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec2 TexCoord;\n"
"out vec3 Normal;\n"
"out vec3 FragPos;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    TexCoord = aTexCoord;\n"
"    Normal = mat3(transpose(inverse(model))) * aNormal; // 正确变换法线\n"
"    FragPos = vec3(model * vec4(aPos, 1.0));\n"
"}\0";

// 球体的片元着色器（用于地球和月亮）
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform sampler2D texture1;\n"
"uniform vec3 lightDir;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"    // 环境光强度\n"
"    float ambientStrength = 0.1;\n"
"    vec3 ambient = ambientStrength * lightColor;\n"
"    // 漫反射计算\n"
"    vec3 norm = normalize(Normal);\n"
"    float diff = max(dot(norm, normalize(lightDir)), 0.0);\n"
"    vec3 diffuse = diff * lightColor * 1.5; // 增加漫反射光强度\n"
"    // 最终颜色\n"
"    vec3 result = (ambient + diffuse) * texture(texture1, TexCoord).rgb;\n"
"    FragColor = vec4(result, 1.0);\n"
"}\0";

// 太阳的顶点着色器
const char* sunVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"    TexCoord = aTexCoord;\n"
"}\0";

// 太阳的片元着色器
const char* sunFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"
"uniform float sunGlowIntensity;\n"
"void main()\n"
"{\n"
"    vec3 texColor = texture(texture1, TexCoord).rgb;\n"
"    FragColor = vec4(texColor * sunGlowIntensity, 1.0);\n"
"}\0";

// 生成球体顶点数据的函数
std::vector<float> generateSphereVertices(float radius, int sectorCount, int stackCount) {
    std::vector<float> vertices;
    float sectorStep = 2 * glm::pi<float>() / sectorCount;
    float stackStep = glm::pi<float>() / stackCount;
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = glm::pi<float>() / 2 - i * stackStep;
        float xy = radius * cos(stackAngle);
        float z = radius * sin(stackAngle);
        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;
            float x = xy * cos(sectorAngle);
            float y = xy * sin(sectorAngle);
            // 顶点位置
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            // 纹理坐标
            vertices.push_back((float)j / sectorCount);
            vertices.push_back((float)i / stackCount);
            // 法线向量
            vertices.push_back(x / radius);
            vertices.push_back(y / radius);
            vertices.push_back(z / radius);
        }
    }
    return vertices;
}

// 生成球体索引数据的函数
std::vector<unsigned int> generateSphereIndices(int sectorCount, int stackCount) {
    std::vector<unsigned int> indices;
    int k1, k2;
    for (int i = 0; i < stackCount; ++i) {
        k1 = i * (sectorCount + 1);
        k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }
            if (i != (stackCount - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
    return indices;
}

// 编译和链接着色器的辅助函数
unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "Error compiling shader: " << infoLog << std::endl;
        return 0;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSource, const char* fragmentSource) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Error linking shader program: " << infoLog << std::endl;
        return 0;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int main() {
    // 初始化 GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1200, 800, "Solar System", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 初始化 GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 创建地球和月亮的着色器程序
    unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);
    if (shaderProgram == 0) {
        std::cout << "Failed to create shader program" << std::endl;
        return -1;
    }

    // 创建太阳的着色器程序
    unsigned int sunShaderProgram = createShaderProgram(sunVertexShaderSource, sunFragmentShaderSource);
    if (sunShaderProgram == 0) {
        std::cout << "Failed to create sun shader program" << std::endl;
        return -1;
    }

    // 生成地球的球体顶点和索引数据
    int sectorCount = 30;
    int stackCount = 30;
    float earthRadius = 1.0f;
    std::vector<float> earthVertices = generateSphereVertices(earthRadius, sectorCount, stackCount);
    std::vector<unsigned int> earthIndices = generateSphereIndices(sectorCount, stackCount);

    // 创建并绑定地球的 VAO, VBO 和 EBO
    unsigned int earthVAO, earthVBO, earthEBO;
    glGenVertexArrays(1, &earthVAO);
    glGenBuffers(1, &earthVBO);
    glGenBuffers(1, &earthEBO);

    glBindVertexArray(earthVAO);

    glBindBuffer(GL_ARRAY_BUFFER, earthVBO);
    glBufferData(GL_ARRAY_BUFFER, earthVertices.size() * sizeof(float), earthVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, earthEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, earthIndices.size() * sizeof(unsigned int), earthIndices.data(), GL_STATIC_DRAW);

    // 顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 法线向量属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 解除绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 加载地球纹理
    unsigned int earthTexture;
    glGenTextures(1, &earthTexture);
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("earth.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load earth texture" << std::endl;
    }
    stbi_image_free(data);

    // 生成月球的球体顶点和索引数据
    float moonRadius = 0.8f * earthRadius;
    std::vector<float> moonVertices = generateSphereVertices(moonRadius, sectorCount, stackCount);
    std::vector<unsigned int> moonIndices = generateSphereIndices(sectorCount, stackCount);

    // 创建并绑定月球的 VAO, VBO 和 EBO
    unsigned int moonVAO, moonVBO, moonEBO;
    glGenVertexArrays(1, &moonVAO);
    glGenBuffers(1, &moonVBO);
    glGenBuffers(1, &moonEBO);

    glBindVertexArray(moonVAO);

    glBindBuffer(GL_ARRAY_BUFFER, moonVBO);
    glBufferData(GL_ARRAY_BUFFER, moonVertices.size() * sizeof(float), moonVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, moonEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, moonIndices.size() * sizeof(unsigned int), moonIndices.data(), GL_STATIC_DRAW);

    // 顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 法线向量属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 解除绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 加载月球纹理
    unsigned int moonTexture;
    glGenTextures(1, &moonTexture);
    glBindTexture(GL_TEXTURE_2D, moonTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("moon.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load moon texture" << std::endl;
    }
    stbi_image_free(data);

    // 生成太阳的球体顶点和索引数据
    float sunRadius = 2.0f;
    std::vector<float> sunVertices = generateSphereVertices(sunRadius, sectorCount, stackCount);
    std::vector<unsigned int> sunIndices = generateSphereIndices(sectorCount, stackCount);

    // 创建并绑定太阳的 VAO, VBO 和 EBO
    unsigned int sunVAO, sunVBO, sunEBO;
    glGenVertexArrays(1, &sunVAO);
    glGenBuffers(1, &sunVBO);
    glGenBuffers(1, &sunEBO);

    glBindVertexArray(sunVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
    glBufferData(GL_ARRAY_BUFFER, sunVertices.size() * sizeof(float), sunVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sunEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sunIndices.size() * sizeof(unsigned int), sunIndices.data(), GL_STATIC_DRAW);

    // 顶点位置属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 纹理坐标属性
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // 法线向量属性
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 解除绑定
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 加载太阳纹理
    unsigned int sunTexture;
    glGenTextures(1, &sunTexture);
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("sun.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load sun texture" << std::endl;
    }
    stbi_image_free(data);

    // 使用正投影矩阵，俯视视角下适当选择视角参数
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1200.0f / 800.0f, 0.1f, 100.0f);

    // 设置视图矩阵，使得相机始终位于太阳上方，俯视整个系统
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 20.0f, 20.0f), // 相机位置
        glm::vec3(0.0f, 0.0f, 0.0f),  // 目标（太阳）
        glm::vec3(0.0f, 1.0f, 0.0f)); // 向上向量

    while (!glfwWindowShouldClose(window)) {
        // 检查并处理事件
        glfwPollEvents();

        // 清除缓冲区
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 绘制太阳
        glUseProgram(sunShaderProgram); // 切换到太阳的着色器程序
        glUniformMatrix4fv(glGetUniformLocation(sunShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(sunShaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform1f(glGetUniformLocation(sunShaderProgram, "sunGlowIntensity"), 0.95f); // 设置太阳发光强度

        // 绑定太阳纹理
        glActiveTexture(GL_TEXTURE0); // 激活纹理单元 0
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glUniform1i(glGetUniformLocation(sunShaderProgram, "texture1"), 0); // 将纹理单元 0 传递给着色器

        glm::mat4 sunModel = glm::mat4(1.0f);
        glUniformMatrix4fv(glGetUniformLocation(sunShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(sunModel));
        glBindVertexArray(sunVAO);
        glDrawElements(GL_TRIANGLES, sunIndices.size(), GL_UNSIGNED_INT, 0);

        // 绘制地球和月亮
        glUseProgram(shaderProgram); // 切换回原来的着色器程序
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        // 设置光照颜色
        glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f); // 白色光
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));

        // 绘制地球
        glm::mat4 earthModel = glm::mat4(1.0f);
        earthModel = glm::rotate(earthModel, (float)glfwGetTime() * glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 公转
        earthModel = glm::translate(earthModel, glm::vec3(10.0f, 0.0f, 0.0f));  // 地球到太阳的距离
        earthModel = glm::rotate(earthModel, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 地球自转
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(earthModel));

        // 计算光源方向（从太阳指向地球）
        glm::vec3 earthPos = glm::vec3(earthModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // 地球的位置
        glm::vec3 lightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - earthPos); // 光源方向
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));

        glBindTexture(GL_TEXTURE_2D, earthTexture);
        glBindVertexArray(earthVAO);
        glDrawElements(GL_TRIANGLES, earthIndices.size(), GL_UNSIGNED_INT, 0);

        // 绘制月亮
        glm::mat4 moonModel = glm::mat4(1.0f);
        glm::mat4 earthCurrentPosition = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime() * glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        earthCurrentPosition = glm::translate(earthCurrentPosition, glm::vec3(10.0f, 0.0f, 0.0f));
        earthCurrentPosition = glm::rotate(earthCurrentPosition, (float)glfwGetTime() * glm::radians(20.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        moonModel = glm::translate(earthCurrentPosition, glm::vec3(2.0f, 0.0f, 0.0f));  // 月亮到地球的距离
        moonModel = glm::rotate(moonModel, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 月亮围绕地球公转
        moonModel = glm::rotate(moonModel, (float)glfwGetTime() * glm::radians(40.0f), glm::vec3(0.0f, 1.0f, 0.0f));  // 月亮自转
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(moonModel));

        // 计算光源方向（从太阳指向月亮）
        glm::vec3 moonPos = glm::vec3(moonModel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)); // 月亮的位置
        lightDir = glm::normalize(glm::vec3(0.0f, 0.0f, 0.0f) - moonPos); // 光源方向
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightDir"), 1, glm::value_ptr(lightDir));

        glBindTexture(GL_TEXTURE_2D, moonTexture);
        glBindVertexArray(moonVAO);
        glDrawElements(GL_TRIANGLES, moonIndices.size(), GL_UNSIGNED_INT, 0);

        // 交换缓冲区
        glfwSwapBuffers(window);
    }

    // 清理资源
    glDeleteVertexArrays(1, &earthVAO);
    glDeleteBuffers(1, &earthVBO);
    glDeleteBuffers(1, &earthEBO);
    glDeleteTextures(1, &earthTexture);

    glDeleteVertexArrays(1, &moonVAO);
    glDeleteBuffers(1, &moonVBO);
    glDeleteBuffers(1, &moonEBO);
    glDeleteTextures(1, &moonTexture);

    glDeleteVertexArrays(1, &sunVAO);
    glDeleteBuffers(1, &sunVBO);
    glDeleteBuffers(1, &sunEBO);
    glDeleteTextures(1, &sunTexture);

    glDeleteProgram(shaderProgram);
    glDeleteProgram(sunShaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}