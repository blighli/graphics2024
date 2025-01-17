#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "camera.h"
#include "model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <ctime>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// 全局常量变量
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 800;
float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera camera(glm::vec3(0.0f, 0.0f, 36.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
const GLfloat PI = 3.14159265358979323846f;
const unsigned int X_SEGMENTS = 60;
const unsigned int Y_SEGMENTS = 60;
std::vector<float> vertices;
std::vector<int> indices;
bool useDirLight = true;
bool usePointLight = true;
bool useSpotLight = true;

// 鼠标回调和输入处理
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    float cameraSpeed = 2.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        useDirLight = !useDirLight;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        usePointLight = !usePointLight;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        useSpotLight = !useSpotLight;
    }
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

// 加载纹理
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    return textureID;
}

// 构造球体
void sphere() {
    for (int y = 0; y <= Y_SEGMENTS; y++) {

        float yAngle = (float)y / (float)Y_SEGMENTS * PI;
        float yPos = std::cos(yAngle);
        float yTex = (float)y / (float)Y_SEGMENTS;
        for (int x = 0; x <= X_SEGMENTS;x++) {
            float xAngle = (float)x / (float)X_SEGMENTS * 2.0f * PI;
            float xPos = std::cos(xAngle) * std::sin(yAngle);
            float zPos = std::sin(xAngle) * std::sin(yAngle);
            float xTex = (float)x / (float)X_SEGMENTS;
            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            vertices.push_back(xPos);
            vertices.push_back(yPos);
            vertices.push_back(zPos);

            vertices.push_back(xTex);
            vertices.push_back(yTex);
        }
    }
    for (int y = 0; y < Y_SEGMENTS; y++) {
        for (int x = 0; x <= X_SEGMENTS;x++) {

            indices.push_back(y * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            indices.push_back(y * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x + 1);
            indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
        }
    }
}

int main() {

    glfwInit();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Complex Model", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowAspectRatio(window, SCR_WIDTH, SCR_HEIGHT);

    // 加载纹理时进行翻转
    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // 点光源
    sphere();
    unsigned int pointLightVAO, VBO, EBO;
    glGenVertexArrays(1, &pointLightVAO);
    glBindVertexArray(pointLightVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    Shader pointLightShader("pointLight.vs", "pointLight.fs");
    Shader lightingShader("lighting.vs", "lighting.fs");


    // 加载模型
    Model nanoModel("assets/nanosuit/nanosuit.obj");


    // 点光源属性
    // 纹理
    unsigned int pointLightTexture = loadTexture("assets/sun.jpg");
    // 坐标 + 大小
    glm::vec3 pointLightPosition(5.0f, 10.0f, 0.0f);
    float pointLight_d = 1.0f;

    glm::mat4 model, view, projection;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setBool("useDirLight", useDirLight);
        lightingShader.setBool("usePointLight", usePointLight);
        lightingShader.setBool("useSpotLight", useSpotLight);
        // 多种光源实现
        // 平行光
        lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
        lightingShader.setVec3("dirLight.ambient", 0.1f, 0.1f, 0.1f);
        lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
        // 点光源
        lightingShader.setVec3("pointLight.position", pointLightPosition);
        lightingShader.setVec3("pointLight.ambient", 0.05f, 0.05f, 0.05f);
        lightingShader.setVec3("pointLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("pointLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("pointLight.constant", 1.0f);
        lightingShader.setFloat("pointLight.linear", 0.09f);
        lightingShader.setFloat("pointLight.quadratic", 0.032f);
        // 聚光
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 6.0f, 6.0f, 6.0f);
        lightingShader.setVec3("spotLight.specular", 6.0f, 6.0f, 6.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.09f);
        lightingShader.setFloat("spotLight.quadratic", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 500.0f);
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        //复杂模型
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -5.0f, 0.0f)); 
        model = glm::scale(model, glm::vec3(1.0f));
        lightingShader.setMat4("model", model);
        nanoModel.Draw(lightingShader);

        // 点光源
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, pointLightTexture);
        pointLightShader.use();
        pointLightShader.setInt("light_texture", 0);
        pointLightShader.setBool("usePointLight", usePointLight);
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPosition);
        model = glm::scale(model, glm::vec3(pointLight_d));
        pointLightShader.setMat4("model", model);
        pointLightShader.setMat4("view", view);
        pointLightShader.setMat4("projection", projection);
        glBindVertexArray(pointLightVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}