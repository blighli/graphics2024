#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SUN_TEXTURE_PATH "/Users/wusttt/XcodeProjects/homework2/homework2/Textures/sun.jpg"
#define EARTH_TEXTURE_PATH "/Users/wusttt/XcodeProjects/homework2/homework2/Textures/earth.jpg"
#define MOON_TEXTURE_PATH "/Users/wusttt/XcodeProjects/homework2/homework2/Textures/moon.jpg"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

const char* vertexShaderSource = R"(
// 顶点着色器
#version 330 core

layout (location = 0) in vec3 aPos; // 顶点坐标
layout (location = 1) in vec3 aNormal; // 法向量
layout (location = 2) in vec2 aTexCoord; // 纹理坐标

out vec3 FragPos; // 片段位置
out vec3 Normal; // 法向量
out vec2 TexCoord; // 纹理坐标

uniform mat4 model; // 模型矩阵
uniform mat4 view; // 视图矩阵
uniform mat4 projection; // 投影矩阵

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // 模型坐标 -> 世界坐标
    Normal = mat3(transpose(inverse(model))) * aNormal; // 转换后的法向量
    TexCoord = aTexCoord;

    gl_Position = projection * view * vec4(FragPos, 1.0); // 最终变换到裁剪坐标
}
)";

const char* fragmentShaderSource = R"(
// 片段着色器
#version 330 core

in vec3 FragPos; // 从顶点着色器传递过来的片段位置
in vec3 Normal; // 从顶点着色器传递过来的法向量
in vec2 TexCoord; // 从顶点着色器传递过来的纹理坐标

out vec4 FragColor; // 输出的片段颜色

uniform sampler2D texture1; // 绑定的纹理
uniform vec3 lightPos; // 光源位置
uniform vec3 lightColor; // 光源颜色
uniform vec3 viewPos;  // 观察者位置

void main() {
    // 环境光照
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // 漫反射光照
    vec3 norm = normalize(Normal); // 单位化法向量
    vec3 lightDir = normalize(lightPos - FragPos); // 光源方向
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // 镜面光照
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // 合成光照颜色
    vec3 lighting = ambient + diffuse + specular;

    // 获取纹理颜色
    vec4 texColor = texture(texture1, TexCoord);

    // 混合光照和纹理颜色
    vec3 result = lighting * texColor.rgb;
    FragColor = vec4(result, texColor.a);
}
)";

void processInput(GLFWwindow* window);
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc);
GLuint loadTexture(const std::string& path);

// camera pose
glm::vec3 cameraPos(-3.0f, 2.0f, 4.0f);
glm::vec3 cameraTarget(1.0f, 0.0f, 0.0f);
glm::vec3 cameraUp(0.1f, 1.0f, 0.0f);

class Sphere {
public:
    GLuint VAO, VBO, EBO;
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    size_t indexCount;

    Sphere(int stacks = 18, int sectors = 36) {
        generateSphereData(stacks, sectors);
        setupSphereBuffers();
    }

    ~Sphere() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    void generateSphereData(int stacks, int sectors) {
        float x, y, z, xy;
        float nx, ny, nz, lengthInv = 1.0f;
        float s, t; // 纹理坐标

        float stackStep = glm::pi<float>() / stacks;
        float sectorStep = 2 * glm::pi<float>() / sectors;

        for (int i = 0; i <= stacks; ++i) {
            float stackAngle = glm::pi<float>() / 2 - i * stackStep;
            xy = cosf(stackAngle);
            z = sinf(stackAngle);

            for (int j = 0; j <= sectors; ++j) {
                float sectorAngle = j * sectorStep;

                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);

                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;
                vertices.push_back(nx);
                vertices.push_back(ny);
                vertices.push_back(nz);

                s = (float)j / sectors;
                t = (float)i / stacks;
                vertices.push_back(s);
                vertices.push_back(t);
            }
        }

        for (int i = 0; i < stacks; ++i) {
            int k1 = i * (sectors + 1);
            int k2 = k1 + sectors + 1;
            for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
                if (i != 0) {
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1 + 1);
                }
                if (i != (stacks - 1)) {
                    indices.push_back(k1 + 1);
                    indices.push_back(k2);
                    indices.push_back(k2 + 1);
                }
            }
        }
        indexCount = indices.size();
    }

    void setupSphereBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        // 顶点位置
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // 法向量
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // 纹理坐标
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
    }
};

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Solar System", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    Sphere sphere;  // create sphere
    
    GLuint sunTexture = loadTexture(SUN_TEXTURE_PATH);
    GLuint earthTexture = loadTexture(EARTH_TEXTURE_PATH);
    GLuint moonTexture = loadTexture(MOON_TEXTURE_PATH);
    
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        
        glm::vec3 lightPosition(0.0f, 5.0f, 4.0f);
        glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
        glm::vec3 cameraPosition = cameraPos;
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPosition));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPosition));
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // sun
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sunTexture);
        glm::mat4 model = glm::mat4(1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sphere.draw();

        // earth
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, earthTexture);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(2.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.5f));
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sphere.draw();

        // moon
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, moonTexture);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f));
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        sphere.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // release resources
    glDeleteTextures(1, &sunTexture);
    glDeleteTextures(1, &earthTexture);
    glDeleteTextures(1, &moonTexture);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

GLuint createShader(const char* source, GLenum type) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, NULL);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Error: Shader Compilation Failed\n" << infoLog << std::endl;
    }
    return shader;
}

GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    GLuint vertexShader = createShader(vertexSrc, GL_VERTEX_SHADER);
    GLuint fragmentShader = createShader(fragmentSrc, GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Error: Shader Linking Failed\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

GLuint loadTexture(const std::string& path) {
    // 加载纹理
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (data) {
        GLenum format = channels == 3 ? GL_RGB : GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
}
