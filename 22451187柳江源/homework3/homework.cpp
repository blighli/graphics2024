#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>

// 顶点和索引数据
std::vector<glm::vec3> vertices;
std::vector<glm::vec3> normals;
std::vector<unsigned int> indices;

// 光源状态
bool redLightEnabled = true;   // 红色光源状态
bool greenLightEnabled = true; // 绿色光源状态
bool blueLightEnabled = true;  // 蓝色光源状态

// 摄像机变量
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f); // 摄像机位置
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // 摄像机前方向量
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f); // 摄像机上方向量
float yaw = -90.0f; // 偏航角
float pitch = 0.0f; // 俯仰角
float lastX = 1920.0f / 2.0f; // 鼠标最后X位置
float lastY = 1440.0f / 2.0f; // 鼠标最后Y位置
float fov = 45.0f; // 视野

// 鼠标控制标志
bool enableMouseMovement = false; // 是否启用鼠标视角更改
bool firstMouse = true; // 是否是第一次鼠标移动
bool leftMouseButtonPressed = false; // 左键是否按下
bool rightMouseButtonPressed = false; // 右键是否按下
glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp)); // 摄像机右方向量

// 摄像机移动标志
float cameraSpeed = 0.05f; // 摄像机移动速度
bool enableCameraMovement = false; // 是否启用摄像机移动

// 模型中心点
glm::vec3 modelCenter = glm::vec3(0.0f, 0.0f, 0.0f);

// 加载模型
void loadModel(const std::string& path) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            aiVector3D vertex = mesh->mVertices[j];
            vertices.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));

            aiVector3D normal = mesh->mNormals[j];
            normals.push_back(glm::vec3(normal.x, normal.y, normal.z));
        }
        for (unsigned int j = 0; j < mesh->mNumFaces; j++) {
            aiFace face = mesh->mFaces[j];
            for (unsigned int k = 0; k < face.mNumIndices; k++) {
                indices.push_back(face.mIndices[k]);
            }
        }
    }

    // 打印加载的顶点和索引数量
    std::cout << "Loaded " << vertices.size() << " vertices and " << indices.size() << " indices." << std::endl;
}

// 加载着色器
GLuint loadShaders(const char* vertexPath, const char* fragmentPath) {
    // 读取顶点着色器代码
    std::string vertexCode;
    std::ifstream vShaderFile(vertexPath);
    if (vShaderFile.is_open()) {
        std::stringstream vShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        vertexCode = vShaderStream.str();
        vShaderFile.close();
    }
    else {
        std::cerr << "Failed to open vertex shader file." << std::endl;
        return 0;
    }

    // 读取片段着色器代码
    std::string fragmentCode;
    std::ifstream fShaderFile(fragmentPath);
    if (fShaderFile.is_open()) {
        std::stringstream fShaderStream;
        fShaderStream << fShaderFile.rdbuf();
        fragmentCode = fShaderStream.str();
        fShaderFile.close();
    }
    else {
        std::cerr << "Failed to open fragment shader file." << std::endl;
        return 0;
    }

    // 编译顶点着色器
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vShaderCode = vertexCode.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);

    // 检查顶点着色器编译错误
    GLint success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "Vertex shader compilation failed: " << infoLog << std::endl;
        return 0;
    }

    // 编译片段着色器
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fShaderCode = fragmentCode.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);

    // 检查片段着色器编译错误
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "Fragment shader compilation failed: " << infoLog << std::endl;
        return 0;
    }

    // 链接着色器程序
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 检查链接错误
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "Shader program linking failed: " << infoLog << std::endl;
        return 0;
    }

    // 删除着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}

// 按键回调函数
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_R:
            redLightEnabled = !redLightEnabled; // 切换红色光源状态
            std::cout << "Red light " << (redLightEnabled ? "enabled" : "disabled") << std::endl;
            break;
        case GLFW_KEY_G:
            greenLightEnabled = !greenLightEnabled; // 切换绿色光源状态
            std::cout << "Green light " << (greenLightEnabled ? "enabled" : "disabled") << std::endl;
            break;
        case GLFW_KEY_B:
            blueLightEnabled = !blueLightEnabled; // 切换蓝色光源状态
            std::cout << "Blue light " << (blueLightEnabled ? "enabled" : "disabled") << std::endl;
            break;
        case GLFW_KEY_P:
            enableMouseMovement = !enableMouseMovement; // 切换鼠标视角更改状态
            std::cout << "Mouse movement " << (enableMouseMovement ? "enabled" : "disabled") << std::endl;
            break;
        case GLFW_KEY_O:
            enableCameraMovement = !enableCameraMovement; // 切换摄像机移动状态
            std::cout << "Camera movement " << (enableCameraMovement ? "enabled" : "disabled") << std::endl;
            break;
        }
    }
}

// 鼠标移动回调函数
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    if (!enableMouseMovement) return; // 如果未启用鼠标视角更改，直接返回

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // 反转Y轴
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // 限制俯仰角
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // 更新摄像机前方向量
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// 鼠标滚轮回调函数
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

// 鼠标按钮回调函数
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMouseButtonPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            leftMouseButtonPressed = false;
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rightMouseButtonPressed = true;
        }
        else if (action == GLFW_RELEASE) {
            rightMouseButtonPressed = false;
        }
    }
}

int main() {
    // 初始化 GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 设置 OpenGL 版本（例如 OpenGL 3.3 Core）
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(1920, 1440, "Press R G B To Switch Light", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(window);

    // 设置按键回调函数
    glfwSetKeyCallback(window, keyCallback);

    // 设置鼠标回调函数
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // 初始化 GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 设置视口大小
    glViewport(0, 0, 1920, 1440);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 加载模型
    loadModel("test.obj");

    // 创建 VAO、VBO 和 EBO
    GLuint VAO, VBO, EBO, NBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glGenBuffers(1, &NBO);

    // 绑定 VAO
    glBindVertexArray(VAO);

    // 将顶点数据复制到 VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    // 将法线数据复制到 NBO
    glBindBuffer(GL_ARRAY_BUFFER, NBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

    // 设置法线属性指针
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(1);

    // 将索引数据复制到 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // 解绑 VAO
    glBindVertexArray(0);

    // 加载着色器
    GLuint shaderProgram = loadShaders("vertex.glsl", "fragment.glsl");
    if (shaderProgram == 0) {
        return -1;
    }

    // 设置材质属性
    struct Material {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };

    Material material;
    material.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    material.diffuse = glm::vec3(0.5f, 0.5f, 0.5f);
    material.specular = glm::vec3(1.0f, 1.0f, 1.0f);
    material.shininess = 32.0f;

    // 设置光源属性
    struct Light {
        glm::vec3 position;
        glm::vec3 direction;
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float constant;
        float linear;
        float quadratic;
        int type; // 0: 点光源, 1: 方向光源
    };

    // 定义光源数量
    const int NUM_LIGHTS = 5; // 2 个原有光源 + 3 个新光源

    Light light[NUM_LIGHTS];

    // 原有光源
    light[0].position = glm::vec3(2.0f, 2.0f, 2.0f); // 点光源
    light[0].ambient = glm::vec3(0.4f, 0.4f, 0.4f); // 环境光强度加倍
    light[0].diffuse = glm::vec3(1.0f, 1.0f, 1.0f); // 漫反射强度加倍
    light[0].specular = glm::vec3(2.0f, 2.0f, 2.0f); // 镜面反射强度加倍
    light[0].constant = 1.0f;
    light[0].linear = 0.09f;
    light[0].quadratic = 0.032f;
    light[0].type = 0; // 点光源

    light[1].direction = glm::vec3(-0.2f, -1.0f, -0.3f); // 方向光
    light[1].ambient = glm::vec3(0.2f, 0.2f, 0.2f);
    light[1].diffuse = glm::vec3(0.8f, 0.8f, 0.8f);
    light[1].specular = glm::vec3(0.9f, 0.9f, 0.9f);
    light[1].type = 1; // 方向光源

    // 新添加的线性光源
    // 红色光源（球体上方）
    light[2].position = glm::vec3(0.0f, 5.0f, 0.0f); // 光源位置
    light[2].direction = glm::vec3(0.0f, -1.0f, 0.0f); // 指向球体
    light[2].ambient = glm::vec3(0.2f, 0.0f, 0.0f); // 红色环境光强度加倍
    light[2].diffuse = glm::vec3(2.0f, 0.0f, 0.0f); // 红色漫反射强度加倍
    light[2].specular = glm::vec3(2.0f, 0.0f, 0.0f); // 红色镜面反射强度加倍
    light[2].constant = 1.0f;
    light[2].linear = 0.18f;
    light[2].quadratic = 0.064f;
    light[2].type = 0; // 点光源

    // 蓝色光源（球体右上方）
    light[3].position = glm::vec3(5.0f, 5.0f, 0.0f); // 光源位置
    light[3].direction = glm::vec3(-1.0f, -1.0f, 0.0f); // 指向球体
    light[3].ambient = glm::vec3(0.0f, 0.0f, 0.2f); // 蓝色环境光强度加倍
    light[3].diffuse = glm::vec3(0.0f, 0.0f, 2.0f); // 蓝色漫反射强度加倍
    light[3].specular = glm::vec3(0.0f, 0.0f, 2.0f); // 蓝色镜面反射强度加倍
    light[3].constant = 1.0f;
    light[3].linear = 0.18f;
    light[3].quadratic = 0.064f;
    light[3].type = 0; // 点光源

    // 绿色光源（球体左侧）
    light[4].position = glm::vec3(-5.0f, 0.0f, 0.0f); // 光源位置
    light[4].direction = glm::vec3(1.0f, 0.0f, 0.0f); // 指向球体
    light[4].ambient = glm::vec3(0.0f, 0.2f, 0.0f); // 绿色环境光强度加倍
    light[4].diffuse = glm::vec3(0.0f, 2.0f, 0.0f); // 绿色漫反射强度加倍
    light[4].specular = glm::vec3(0.0f, 2.0f, 0.0f); // 绿色镜面反射强度加倍
    light[4].constant = 1.0f;
    light[4].linear = 0.18f;
    light[4].quadratic = 0.064f;
    light[4].type = 0; // 点光源

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 处理摄像机移动
        if (enableCameraMovement) {
            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                cameraPos += cameraSpeed * cameraFront; // 向前移动
            }
            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                cameraPos -= cameraSpeed * cameraFront; // 向后移动
            }
            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; // 向左移动
            }
            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; // 向右移动
            }
        }

        // 清除颜色缓冲区和深度缓冲区
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器程序
        glUseProgram(shaderProgram);

        // 设置变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f)); // 将模型向后移动
        model = glm::scale(model, glm::vec3(0.5f)); // 缩小模型

        // 更新视图矩阵
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp); // 以摄像机为中心

        // 更新投影矩阵
        glm::mat4 projection = glm::perspective(glm::radians(fov), 1920.0f / 1440.0f, 0.1f, 100.0f);

        GLuint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        // 传递材质属性
        GLuint matAmbientLoc = glGetUniformLocation(shaderProgram, "material.ambient");
        GLuint matDiffuseLoc = glGetUniformLocation(shaderProgram, "material.diffuse");
        GLuint matSpecularLoc = glGetUniformLocation(shaderProgram, "material.specular");
        GLuint matShininessLoc = glGetUniformLocation(shaderProgram, "material.shininess");
        glUniform3fv(matAmbientLoc, 1, glm::value_ptr(material.ambient));
        glUniform3fv(matDiffuseLoc, 1, glm::value_ptr(material.diffuse));
        glUniform3fv(matSpecularLoc, 1, glm::value_ptr(material.specular));
        glUniform1f(matShininessLoc, material.shininess);

        // 传递光源属性
        for (int i = 0; i < NUM_LIGHTS; i++) {
            std::string lightStr = "light[" + std::to_string(i) + "]";
            glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + ".position").c_str()), 1, glm::value_ptr(light[i].position));
            glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + ".direction").c_str()), 1, glm::value_ptr(light[i].direction));
            glUniform1i(glGetUniformLocation(shaderProgram, (lightStr + ".type").c_str()), light[i].type); // 传递光源类型

            // 根据光源状态调整光源强度
            glm::vec3 ambient = light[i].ambient;
            glm::vec3 diffuse = light[i].diffuse;
            glm::vec3 specular = light[i].specular;

            if (i == 2 && !redLightEnabled) { // 红色光源
                ambient = glm::vec3(0.0f);
                diffuse = glm::vec3(0.0f);
                specular = glm::vec3(0.0f);
            }
            if (i == 3 && !blueLightEnabled) { // 蓝色光源
                ambient = glm::vec3(0.0f);
                diffuse = glm::vec3(0.0f);
                specular = glm::vec3(0.0f);
            }
            if (i == 4 && !greenLightEnabled) { // 绿色光源
                ambient = glm::vec3(0.0f);
                diffuse = glm::vec3(0.0f);
                specular = glm::vec3(0.0f);
            }

            glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + ".ambient").c_str()), 1, glm::value_ptr(ambient));
            glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + ".diffuse").c_str()), 1, glm::value_ptr(diffuse));
            glUniform3fv(glGetUniformLocation(shaderProgram, (lightStr + ".specular").c_str()), 1, glm::value_ptr(specular));
            glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + ".constant").c_str()), light[i].constant);
            glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + ".linear").c_str()), light[i].linear);
            glUniform1f(glGetUniformLocation(shaderProgram, (lightStr + ".quadratic").c_str()), light[i].quadratic);
        }

        // 传递摄像机位置
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        // 绑定 VAO 并绘制模型
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 交换缓冲区
        glfwSwapBuffers(window);

        // 处理事件
        glfwPollEvents();
    }

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &NBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}