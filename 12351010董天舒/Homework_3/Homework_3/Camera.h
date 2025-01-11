// Camera.h
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// 定义相机可能的移动方向
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// 默认相机参数
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 5.0f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;

class Camera {
public:
    // 相机属性
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // 欧拉角
    float Yaw;
    float Pitch;

    // 相机参数
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // 构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW, float pitch = PITCH);

    // 返回视图矩阵
    glm::mat4 GetViewMatrix();

    // 处理键盘输入
    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);

    // 处理鼠标滚轮
    void ProcessMouseScroll(float yoffset);

private:
    // 更新相机的向量
    void updateCameraVectors();
};

#endif
