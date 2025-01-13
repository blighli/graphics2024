// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>

// 回调函数声明
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// 屏幕设置
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// 相机
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f; // 当前帧与上一帧的时间差
float lastFrame = 0.0f;

int main() {
    // 初始化GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 配置GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // 创建窗口
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Model Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // 设置回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 捕获鼠标
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 初始化GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // 编译着色器
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // 加载模型
    Model model("models/bunny_10k.obj");

    // 设置光源
    glm::vec3 lightPositions[] = {
        glm::vec3(0.0f,  10.0f,  0.0f),
        glm::vec3(10.0f, 0.0f,  10.0f),
        glm::vec3(-10.0f, 0.0f,  10.0f),
        glm::vec3(0.0f, -10.0f, 0.0f)
    };
    glm::vec3 lightColors[] = {
        glm::vec3(1.0f, 0.5f, 0.5f),
        glm::vec3(0.5f, 1.0f, 0.5f),
        glm::vec3(0.5f, 0.5f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)
    };

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        // 计算时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 输入
        processInput(window);

        // 渲染
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 激活着色器
        shader.use();

        // 设置相机矩阵
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // 设置模型矩阵（模型中心的平移、旋转和缩放）
        glm::mat4 modelMat = glm::mat4(1.0f);
        // 示例：旋转
        modelMat = glm::rotate(modelMat, glm::radians((float)glfwGetTime() * 20.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", modelMat);

        // 设置光源
        for (int i = 0; i < 4; i++) {
            std::string number = std::to_string(i);
            shader.setVec3("lights[" + number + "].position", lightPositions[i]);
            shader.setVec3("lights[" + number + "].ambient", lightColors[i] * 0.1f);
            shader.setVec3("lights[" + number + "].diffuse", lightColors[i]);
            shader.setVec3("lights[" + number + "].specular", glm::vec3(1.0f));
        }

        // 设置观察者位置
        shader.setVec3("viewPos", camera.Position);

        // 绘制模型，并传递 shader 对象
        model.Draw(shader);

        // 交换缓冲区和轮询IO事件
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 终止GLFW
    glfwTerminate();
    return 0;
}

// 处理所有输入
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// 窗口尺寸改变回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 鼠标移动回调
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y方向反转

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 滚轮回调
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
