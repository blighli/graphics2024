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
const GLfloat SENSITIVITY = 0.25f;//缩放的快慢程度

//一个抽象的camera类，用于处理输入并计算OpenGL中使用的相应Euler角（欧拉角）、向量和矩阵
class Camera
{
public:
	//相机属性
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	//欧拉角
	GLfloat Pitch;//俯仰
	GLfloat Yaw;  //偏航
	GLfloat Roll; //翻滚
	//相机选项
	GLfloat MovementSpeed;
	GLfloat MouseSensitivity;//鼠标偏移量系数
	GLfloat Zoom;

	//使用向量构造
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), GLfloat pitch = PITCH, GLfloat yaw = YAW) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = position;
		this->WorldUp = up;
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//使用标量构造
	Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY, GLfloat upZ, GLfloat pitch, GLfloat yaw) : Front(glm::vec3(0.0f, 0.0f, -2.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		this->Position = glm::vec3(posX, posY, posZ);
		this->WorldUp = glm::vec3(upX, upY, upZ);
		this->Pitch = pitch;
		this->Yaw = yaw;
		this->UpdateCameraVectors();
	}
	//返回使用Eular角和LookAt矩阵计算的视图矩阵
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(this->Position, this->Position + this->Front, this->Up);
	}
	//处理从任何类似键盘的输入系统接收的输入。接受摄像机定义枚举形式的输入参数（从窗口系统中提取）
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
	//处理从鼠标输入系统接收的输入。在x和y方向都需要偏移值。
	void ProcessMouseMovement(GLfloat xOffset, GLfloat yOffset, GLboolean constrainPitch = true)
	{
		xOffset *= this->MouseSensitivity;
		yOffset *= this->MouseSensitivity;

		this->Yaw += xOffset;  //偏航，左右是X轴
		this->Pitch += yOffset;//俯仰，上下是Y轴

		//确保当俯仰角超出+-90度，屏幕不会翻转
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

		//使用更新的Eular角度更新前、右和上向量
		this->UpdateCameraVectors();
	}

	//处理从鼠标滚轮事件接收的输入。只需要输入垂直车轮轴
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
	//根据相机（更新的）Eular角度计算前向量
	void UpdateCameraVectors()
	{
		//计算新的前向量
		glm::vec3 front;
		front.x = cos(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		front.y = sin(glm::radians(this->Pitch));
		front.z = sin(glm::radians(this->Yaw)) * cos(glm::radians(this->Pitch));
		this->Front = glm::normalize(front);//attention
		//同时重新计算右上向量
		//规范化向量，因为向量的长度越接近0，向上或向下查找的次数越多，移动速度就越慢。
		this->Right = glm::normalize(glm::cross(this->Front, this->WorldUp));
		this->Up = glm::normalize(glm::cross(this->Right, this->Front));  //cross如果参数交换，方向相反
	}
};


#endif // !_CAMERA_H_
