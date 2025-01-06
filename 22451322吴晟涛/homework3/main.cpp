#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define OBJ_PATH "/Users/wusttt/XcodeProjects/homework3/homework3/mesh/bunny_10k.obj"
#define TEXTURE_PATH "/Users/wusttt/XcodeProjects/homework3/homework3/mesh/bunny_10k_texture.jpg"

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

float yaw = -90.0f; // 初始水平角度
float pitch = 0.0f; // 初始垂直角度
float lastX = WIDTH / 2.0f; // 鼠标初始位置
float lastY = HEIGHT / 2.0f;
float fov = 45.0f; // 初始视野范围

bool firstMouse = true;

// 相机速度
const float cameraSpeed = 2.5f;
const float sensitivity = 0.1f;

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
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
GLuint createShaderProgram(const char* vertexSrc, const char* fragmentSrc);
GLuint loadTexture(const std::string& path);

// camera pose
glm::vec3 cameraPos(0.0f, 2.0f, 3.0f);
glm::vec3 cameraTarget(0.0f, 0.5f, 0.0f);
glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

class ObjModel {
public:
    GLuint VAO, VBO;
    std::vector<float> vertices;

    ObjModel(const std::string& objPath) {
        loadObj(objPath);
        setupBuffers();
    }

    ~ObjModel() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void draw() const {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 8);
        glBindVertexArray(0);
    }

private:
    void loadObj(const std::string& objPath) {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objPath.c_str())) {
            throw std::runtime_error("Failed to load .obj file: " + objPath + "\n" + warn + err);
        }

        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                tinyobj::real_t vx = attrib.vertices[3 * index.vertex_index + 0];
                tinyobj::real_t vy = attrib.vertices[3 * index.vertex_index + 1];
                tinyobj::real_t vz = attrib.vertices[3 * index.vertex_index + 2];
                vertices.push_back(vx);
                vertices.push_back(vy);
                vertices.push_back(vz);

                if (!attrib.normals.empty()) {
                    tinyobj::real_t nx = attrib.normals[3 * index.normal_index + 0];
                    tinyobj::real_t ny = attrib.normals[3 * index.normal_index + 1];
                    tinyobj::real_t nz = attrib.normals[3 * index.normal_index + 2];
                    vertices.push_back(nx);
                    vertices.push_back(ny);
                    vertices.push_back(nz);
                } else {
                    vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f});
                }

                if (!attrib.texcoords.empty()) {
                    tinyobj::real_t tx = attrib.texcoords[2 * index.texcoord_index + 0];
                    tinyobj::real_t ty = attrib.texcoords[2 * index.texcoord_index + 1];
                    vertices.push_back(tx);
                    vertices.push_back(ty);
                } else {
                    vertices.insert(vertices.end(), {0.0f, 0.0f});
                }
            }
        }
    }

    void setupBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

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

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 捕获鼠标
    
    glEnable(GL_DEPTH_TEST);

    GLuint shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    ObjModel objModel(OBJ_PATH); // 加载mesh和texture
    GLuint texture = loadTexture(TEXTURE_PATH);

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
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        objModel.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

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
    // 处理键盘输入
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    float currentSpeed = cameraSpeed * 0.05f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += currentSpeed * cameraTarget;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= currentSpeed * cameraTarget;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraTarget, cameraUp)) * currentSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraTarget, cameraUp)) * currentSpeed;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f) fov = 1.0f;
    if (fov > 45.0f) fov = 45.0f;
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // 处理鼠标输入
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraTarget = glm::normalize(front);
}

GLuint loadTexture(const std::string& path) {
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
