#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

// 结构体表示顶点数据
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

// 加载 OBJ 模型
std::vector<Vertex> loadModel(const char* path) {
    std::vector<Vertex> vertices;
    Assimp::Importer importer;

    // 使用 Assimp 加载模型
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error: Assimp could not load model!" << std::endl;
        return vertices;
    }

    aiMesh* mesh = scene->mMeshes[0];  // 假设只有一个网格

    // 获取顶点数据
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

        if (mesh->HasNormals()) {
            vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }

        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    return vertices;
}

// 加载并设置 VBO 和 VAO
void setupMesh(const std::vector<Vertex>& vertices, GLuint& VAO, GLuint& VBO) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // 顶点位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);

    // 法线
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // 纹理坐标
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);  // 解绑 VAO
}

// 渲染模型
void renderMesh(GLuint VAO, size_t indexCount) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, indexCount);  // 假设每个模型是由三角形组成
    glBindVertexArray(0);
}

// 着色器加载函数
GLuint LoadShader(const char* shaderPath, GLenum shaderType) {
    FILE* file = nullptr;
    errno_t err = fopen_s(&file, shaderPath, "r");
    if (err != 0) {
        // 错误处理：文件未能打开
        std::cerr << "Error opening file!" << std::endl;
    }

    if (!file) {
        std::cerr << "Error: Shader file not found!" << std::endl;
        return 0;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* source = new char[length + 1];
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    delete[] source;

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Error: Shader compilation failed\n" << infoLog << std::endl;
        return 0;
    }

    return shader;
}

// 着色器程序加载
GLuint LoadShaderProgram(const char* vertexPath, const char* fragmentPath) {
    GLuint vertexShader = LoadShader(vertexPath, GL_VERTEX_SHADER);
    GLuint fragmentShader = LoadShader(fragmentPath, GL_FRAGMENT_SHADER);
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Error: Program linking failed\n" << infoLog << std::endl;
    }

    return shaderProgram;
}

// 设置光源
struct Light {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
};

Light lights[3] = {
    {{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f}, 1.0f},
    {{-1.0f, 1.0f, 1.0f}, {1.0f, 0.5f, 0.5f}, 0.8f},
    {{0.0f, 2.0f, -2.0f}, {0.5f, 0.5f, 1.0f}, 0.6f}
};

int main() {
    if (!glfwInit()) {
        std::cerr << "Error: GLFW initialization failed!" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Lighting Project", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error: GLFW window creation failed!" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewInit();

    // 加载模型
    std::vector<Vertex> vertices = loadModel("model/drone_costum.obj");

    GLuint VAO, VBO;
    setupMesh(vertices, VAO, VBO);

    // 加载着色器程序
    GLuint shaderProgram = LoadShaderProgram("vertex_shader_other.glsl", "fragment_shader_other.glsl");
    glUseProgram(shaderProgram);

    // 设置视图矩阵和投影矩阵
    glm::vec3 cameraPos(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 计算视图和投影矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // 传递矩阵到着色器
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // 设置光源数据
        for (int i = 0; i < 3; i++) {
            std::string uniformName = "lights[" + std::to_string(i) + "].position";
            glUniform3fv(glGetUniformLocation(shaderProgram, uniformName.c_str()), 1, glm::value_ptr(lights[i].position));

            uniformName = "lights[" + std::to_string(i) + "].color";
            glUniform3fv(glGetUniformLocation(shaderProgram, uniformName.c_str()), 1, glm::value_ptr(lights[i].color));

            uniformName = "lights[" + std::to_string(i) + "].intensity";
            glUniform1f(glGetUniformLocation(shaderProgram, uniformName.c_str()), lights[i].intensity);
        }

        // 渲染模型
        renderMesh(VAO, vertices.size());


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
