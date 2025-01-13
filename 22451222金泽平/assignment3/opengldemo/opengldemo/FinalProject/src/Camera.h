//
//  Camera.h
//  CG3
//
//  Created by rui huang on 2024/11/25.
//

#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};
const GLfloat YAW    = -90.0f;
const GLfloat PITCH  = 0.0f;
const GLfloat ZOOM   = 45.0f;

class Camera
{
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat yaw = YAW, GLfloat pitch = PITCH):front(glm::vec3(0.0f, 0.0f, -1.0f)), zoom(ZOOM)
    {
        this->position = position;
        this->worldUp = up;
        this->yaw = yaw;
        this->pitch = pitch;
        updateCameraVectors();
    }
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt( position, position+front, up);
    }
    GLfloat GetZoom()
    {
        return zoom;
    }
    void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
    {
        GLfloat velocity = 6.0f * deltaTime;
        if (direction == FORWARD) {
            position += front*velocity;
        }
        if (direction == BACKWARD) {
            position -= front*velocity;
        } 
        if (direction == LEFT) {
            position -= right*velocity;
        }
        if (direction == RIGHT) {
            position += right*velocity;
        }
    }
    void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset)
    {
        GLfloat sensitivity = 0.2f;
        xOffset *= sensitivity;
        yOffset *= sensitivity;
        yaw += xOffset;
        pitch += yOffset;
        updateCameraVectors();
    }
private:
    GLfloat yaw, pitch, zoom;
    glm::vec3 position, front, up, right, worldUp;
    void updateCameraVectors()
    {
        glm::vec3 front;
        front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
        front.y = sin(glm::radians(pitch));
        front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
        this->front = glm::normalize(front);
        this->right = glm::normalize(glm::cross(this->front, this->worldUp));
        this->up = glm::normalize(glm::cross(this->right, this->front));
    }
};
