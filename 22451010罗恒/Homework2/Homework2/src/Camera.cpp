#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include<glm/gtc/matrix_transform.hpp>


Camera::Camera(glm::vec3 cameraPosition)
{
    m_Fov = 90.0f;
    m_Yaw = -90.0f;
    m_Pitch = 0.0f;
    m_Position = cameraPosition;
    m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
    reTargetCamera();
}

void Camera::InputProcessKey(INPUTKEY inputKey, const float& deltaTime)
{
    float offset = deltaTime * m_MoveSpeed;
    switch (inputKey)
    {
    case INPUTKEY::KEY_A:
        m_Position -= m_Right * offset;
        break;
    case INPUTKEY::KEY_D:
        m_Position += m_Right * offset;
        break;
    case INPUTKEY::KEY_W:
        m_Position += m_Front * offset;
        break;
    case INPUTKEY::KEY_S:
        m_Position -= m_Front * offset;
        break;
    }
}

void Camera::InputProcessMouse(float yawOffset, float pitchOffset )
{
    pitchOffset = pitchOffset * m_DirecSpeed;
    yawOffset = yawOffset * m_DirecSpeed;
    m_Yaw += yawOffset;
    m_Pitch += pitchOffset;
    if (m_Pitch > 89.0f)
    {
        m_Pitch = 89.0f;
    }
    else if (m_Pitch < -89.0f)
    {
        m_Pitch = -89.0f;
    }
   
    reTargetCamera();
}

void Camera::InputProcessFov(float yoffset)
{
    m_Fov += yoffset;
    if (m_Fov < 1.0f) m_Fov = 1.0f;
    else if (m_Fov > 90.0f) m_Fov = 90.0f;
}

glm::vec3 Camera::getPosition()
{
    return m_Position;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

glm::mat4 Camera::getPerspective(float aspect)
{
    return glm::perspective(glm::radians(m_Fov), aspect, 0.1f, 100.0f);
}

void Camera::reTargetCamera()
{
    m_Front.z = glm::sin(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    m_Front.x = glm::cos(glm::radians(m_Yaw)) * glm::cos(glm::radians(m_Pitch));
    m_Front.y = glm::sin(glm::radians(m_Pitch));
    m_Front = glm::normalize(m_Front);

    m_Right = glm::normalize(glm::cross(m_Front, worldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}
