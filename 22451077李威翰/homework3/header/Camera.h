#pragma once
#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "GL/glew.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <iostream>

using namespace std;

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const GLfloat PITCH = 0.0f;
const GLfloat YAW = -90.0f;
const GLfloat ROLL = 0.0f;
const GLfloat SPEED = 36.0f;
const GLfloat ZOOM = 45.0f;
const GLfloat SENSITIVITY = 0.25f;//���ŵĿ����̶�

//һ�������camera�࣬���ڴ������벢����OpenGL��ʹ�õ���ӦEuler�ǣ�ŷ���ǣ��������;���
class Camera
{
public:
	//�������
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//ŷ����
	GLfloat Pitch;//����
	GLfloat Yaw;  //ƫ��
	GLfloat Roll; //����
	//���ѡ��
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;//���ƫ����ϵ��
	GLfloat Zoom;

	//ʹ����������
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat pitch = PITCH, GLfloat yaw = YAW) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//ʹ�ñ�������
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat pitch, GLfloat yaw) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//����ʹ��Eular�Ǻ�LookAt����������ͼ����
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	//������κ����Ƽ��̵�����ϵͳ���յ����롣�������������ö����ʽ������������Ӵ���ϵͳ����ȡ��
	void ProcessKeyboard(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->MovementSpeed * deltaTime * 0.1;
		if (direction == FORWARD)
		{
			this->Position += this->Front * velocity;
		}
		if (direction == BACKWARD)
		{
			this->Position -= this->Front * velocity;
		}
		if (direction == LEFT)
		{
			this->Position -= this->Right * velocity;
			//this->Position -= glm::cross(this->Front, this->Up) * velocity;
		}
		if (direction == RIGHT)
		{
			this->Position += this->Right * velocity;
			//this->Position += glm::cross(this->Front, this->Up) * velocity;
		}
	}
	//������������ϵͳ���յ����롣��x��y������Ҫƫ��ֵ��
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= this->MouseSensitivity;
		yOffset *= this->MouseSensitivity;

		this->Yaw += xOffset;  //ƫ����������X��
		this->Pitch += yOffset;//������������Y��

		//ȷ���������ǳ���+-90�ȣ���Ļ���ᷭת
		if (constrainPitch)
		{
			if (this->Pitch > 89.0f)
			{
				this->Pitch = 89.0f;
			}
			if (this->Pitch < -89.0f)
			{
				this->Pitch = -89.0f;
			}
		}

		//ʹ�ø��µ�Eular�Ƕȸ���ǰ���Һ�������
		this->UpdateCameraVectors();
	}

	//������������¼����յ����롣ֻ��Ҫ���봹ֱ������
	void ProcessMouseScroll(GLfloat zOffset)
	{
		if (this->Zoom >= 1.0f && this->Zoom <= 45.0f)
		{
			this->Zoom -= zOffset;
		}
		if (this->Zoom <= 1.0f)
		{
			this->Zoom = 1.0f;
		}
		if (this->Zoom >= 45.0f)
		{
			this->Zoom = 45.0f;
		}
	}
protected:
private:
	//������������µģ�Eular�Ƕȼ���ǰ����
	void UpdateCameraVectors()
	{
		//�����µ�ǰ����
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);//attention
		//ͬʱ���¼�����������
		//�淶����������Ϊ�����ĳ���Խ�ӽ�0�����ϻ����²��ҵĴ���Խ�࣬�ƶ��ٶȾ�Խ����
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));  //cross������������������෴
	}
};


#endif // !_CAMERA_H_
