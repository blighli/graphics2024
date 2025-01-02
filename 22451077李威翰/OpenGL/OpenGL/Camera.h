#pragma once
#include <glm/glm.hpp>

enum class INPUTKEY
{
	KEY_A = 1, KEY_D = 2,
	KEY_S = 3, KEY_W = 4
};
class Camera
{
public:
	Camera(glm::vec3 cameraPosition);
	void InputProcessKey(INPUTKEY inputKey, const float& deltaTime);
	void InputProcessMouse(float yawOffset, float pitchOffset);
	void InputProcessFov(float yoffset);
	glm::vec3 getPosition();
	glm::mat4 getViewMatrix();
	glm::mat4 getPerspective(float aspect);
private:
	float m_Fov = 45.0f;
	float m_Yaw = -90.0f;
	float m_Pitch = 0.0f;
	float m_MoveSpeed = 5.0f;
	float m_DirecSpeed = 0.5f;
	glm::vec3 m_Position;
	glm::vec3 m_Up;
	glm::vec3 m_Right;
	glm::vec3 m_Front;
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
	void reTargetCamera();
}; 