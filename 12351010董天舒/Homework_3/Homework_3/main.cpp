// main.cpp
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Shader.h"
#include "Model.h"
#include "Camera.h"
#include <iostream>

// �ص���������
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// ��Ļ����
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// ���
Camera camera;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// ʱ��
float deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
float lastFrame = 0.0f;

int main() {
    // ��ʼ��GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // ����GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // ��������
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Model Viewer", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // ���ûص�����
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // �������
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ��ʼ��GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // ������Ȳ���
    glEnable(GL_DEPTH_TEST);

    // ������ɫ��
    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // ����ģ��
    Model model("models/bunny_10k.obj");

    // ���ù�Դ
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

    // ��Ⱦѭ��
    while (!glfwWindowShouldClose(window)) {
        // ����ʱ��
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ����
        processInput(window);

        // ��Ⱦ
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ������ɫ��
        shader.use();

        // �����������
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
            (float)SCR_WIDTH / (float)SCR_HEIGHT,
            0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // ����ģ�;���ģ�����ĵ�ƽ�ơ���ת�����ţ�
        glm::mat4 modelMat = glm::mat4(1.0f);
        // ʾ������ת
        modelMat = glm::rotate(modelMat, glm::radians((float)glfwGetTime() * 20.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", modelMat);

        // ���ù�Դ
        for (int i = 0; i < 4; i++) {
            std::string number = std::to_string(i);
            shader.setVec3("lights[" + number + "].position", lightPositions[i]);
            shader.setVec3("lights[" + number + "].ambient", lightColors[i] * 0.1f);
            shader.setVec3("lights[" + number + "].diffuse", lightColors[i]);
            shader.setVec3("lights[" + number + "].specular", glm::vec3(1.0f));
        }

        // ���ù۲���λ��
        shader.setVec3("viewPos", camera.Position);

        // ����ģ�ͣ������� shader ����
        model.Draw(shader);

        // ��������������ѯIO�¼�
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ��ֹGLFW
    glfwTerminate();
    return 0;
}

// ������������
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

// ���ڳߴ�ı�ص�
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// ����ƶ��ص�
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // y����ת

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// ���ֻص�
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}
