#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define SCREENWIDTH 800
#define SCREENHEIGHT 600

Camera mainCamera(glm::vec3(0.0, 0.0, 50.0));
glm::vec2 lastPos((float)SCREENWIDTH / 2.0f, (float)SCREENHEIGHT / 2.0f);
bool firstMoveIn = true;
void processInput(GLFWwindow* window, float deltaTime);
void processCursorMove(GLFWwindow* window, double xpos, double ypos);
void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);
void renderSphere();
uint32_t getTexID(const std::string& path);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_COMPAT_PROFILE, GLFW_OPENGL_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "Solar System", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "error load glad" << std::endl;
		return EXIT_FAILURE;
	}

	//set window callback
	glfwSetCursorPosCallback(window, processCursorMove);

	// Set mouse button callback
	glfwSetMouseButtonCallback(window, mouseClickCallback);

	//shader
	Shader sphereShader("vertex_shader.glsl", "fragment_shader.glsl");
	Shader earthShader("vertex_shader_other.glsl", "fragment_shader_other.glsl");

	//texture
	uint32_t sunTex = getTexID("texture/sun.jpg");
	uint32_t earthTex = getTexID("texture/earth.jpg");
	uint32_t moonTex = getTexID("texture/moon.jpg");
	sphereShader.use();
	sphereShader.setValue("sphereTex", 0);

	//uniform buffer
	uint32_t ubo;
	{
		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, ubo, 0, sizeof(glm::mat4) * 2);
	}

	float lastTime = (float)glfwGetTime();
	glm::vec3 sunPos = glm::vec3(0.0, 0.0, 0.0);
	float sunRadius = 90.0f;
	glm::mat4 sunmodel = glm::mat4(1.0);


	sunmodel = glm::mat4(1.0);
	sunmodel = glm::translate(sunmodel, glm::vec3(-60.0f, 10.0f, 0.0f));
	sunmodel = glm::scale(sunmodel, glm::vec3(30.0f, 30.0f, 30.0f));

	glm::vec4 sunPosPas = sunmodel * glm::vec4(sunPos, 1.0f);
	sunPos = glm::vec3(sunPosPas / sunPosPas.w);

	while (!glfwWindowShouldClose(window))
	{
		float current = (float)glfwGetTime();
		float deltaTime = current - lastTime;
		lastTime = current;
		processInput(window, deltaTime);
		glEnable(GL_DEPTH_TEST);
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glm::mat4 PMatrix[2] = { mainCamera.getViewMatrix(), mainCamera.getPerspective((float)SCREENWIDTH / (float)SCREENHEIGHT) };
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4) * 2, glm::value_ptr(PMatrix[0]));

		//render sphere
		sphereShader.use();
		//rendersun
		sphereShader.setValue("model", sunmodel);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sunTex);
		renderSphere();

		//render earth
		earthShader.use();
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
		earthShader.setValue("model", model);
		earthShader.setValue("TangentToWorld", glm::transpose(glm::inverse(glm::mat3(model))));
		earthShader.setValue("tex", 0);
		earthShader.setValue("sunTex", 1);

		earthShader.setValue("sunPos", sunPos);
		earthShader.setValue("sunRadius", sunRadius);
		earthShader.setValue("camearPos", mainCamera.getPosition());

		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, -20.0f));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, earthTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sunTex);
		renderSphere();

		//render moon
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(20.0f, 0.0f, -10.0f));
		model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

		earthShader.setValue("model", model);
		earthShader.setValue("TangentToWorld", glm::transpose(glm::inverse(glm::mat3(model))));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, moonTex);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sunTex);
		renderSphere();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

void processInput(GLFWwindow* window, float deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_A, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_D, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_W, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		mainCamera.InputProcessKey(INPUTKEY::KEY_S, deltaTime);
	}
}

void processCursorMove(GLFWwindow* window, double xpos, double ypos)
{
	float xPos = static_cast<float>(xpos);
	float yPos = static_cast<float>(ypos);

	if (firstMoveIn)
	{
		firstMoveIn = false;
		lastPos.x = xPos;
		lastPos.y = yPos;
	}
	float pitchMove = lastPos.y - yPos;
	float yawMove = xPos - lastPos.x;
	lastPos = glm::vec2(xPos, yPos);
	mainCamera.InputProcessMouse(yawMove, pitchMove);
}


uint32_t sphereVAO = 0;
uint32_t sphereVerCount;
void renderSphere()
{
	if (sphereVAO == 0)
	{
		uint32_t sphereVBO;
		uint32_t sphereEBO;
		glGenVertexArrays(1, &sphereVAO);
		glGenBuffers(1, &sphereVBO);
		glGenBuffers(1, &sphereEBO);

		std::vector<glm::vec3> position;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normal;
		std::vector<uint32_t> indices;

		const unsigned int X_SEGMENT = 64;
		const unsigned int Y_SEGMENT = 64;

		const float PI = 3.1415926f;

		for (uint32_t x = 0; x <= X_SEGMENT; x++)
		{
			for (uint32_t y = 0; y <= Y_SEGMENT; y++)
			{
				float theta = PI * ((float)y / (float)Y_SEGMENT);
				float phi = 2 * PI * ((float)x / (float)X_SEGMENT);

				float xpos = std::sin(theta) * std::cos(phi);
				float ypos = std::cos(theta);
				float zpos = std::sin(theta) * std::sin(phi);

				position.push_back(glm::vec3(xpos, ypos, zpos));
				uv.push_back(glm::vec2(
					(float)x / (float)X_SEGMENT,
					(float)y / (float)Y_SEGMENT
				));
				normal.push_back(glm::vec3(xpos, ypos, zpos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENT; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENT; ++x)
				{
					indices.push_back(y * (X_SEGMENT + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENT + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENT; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENT + 1) + x);
					indices.push_back(y * (X_SEGMENT + 1) + x);
				}
			}
			oddRow = !oddRow;
		}

		sphereVerCount = indices.size();

		std::vector<float> data;
		for (int i = 0; i < position.size(); i++)
		{
			data.push_back(position[i].x);
			data.push_back(position[i].y);
			data.push_back(position[i].z);
			if (normal.size() > 0)
			{
				data.push_back(normal[i].x);
				data.push_back(normal[i].y);
				data.push_back(normal[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
		glBindVertexArray(sphereVAO);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
		glBindVertexArray(0);
	}

	glBindVertexArray(sphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, sphereVerCount, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

uint32_t getTexID(const std::string& path)
{
	uint32_t texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	int width, height, channel;
	stbi_set_flip_vertically_on_load(true);
	stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channel, 0);
	GLenum interFormat, dataFormat;
	if (!data)
	{
		std::cerr << "error load texture" << std::endl;
		return 0;
	}
	switch (channel)
	{
	case 3:
		interFormat = GL_SRGB, dataFormat = GL_RGB;
		break;
	case 4:
		interFormat = GL_SRGB_ALPHA, dataFormat = GL_RGBA;
		break;
	}
	glTexImage2D(GL_TEXTURE_2D, 0, interFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_2D, texID);
	return texID;
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		// 获取视口和投影矩阵
		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glm::mat4 projection = mainCamera.getPerspective((float)SCREENWIDTH / (float)SCREENHEIGHT);
		glm::mat4 view = mainCamera.getViewMatrix();

		// 转换鼠标坐标到 NDC (Normalized Device Coordinates)
		float ndcX = (2.0f * (float)xpos) / (float)viewport[2] - 1.0f;
		float ndcY = 1.0f - (2.0f * (float)ypos) / (float)viewport[3];
		float ndcZ = 0.0f; // 假设深度值接近前方

		// 转换为世界坐标
		glm::vec4 clipCoords = glm::vec4(ndcX, ndcY, -1.0f, 1.0f);
		glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
		eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

		glm::vec4 worldCoords = glm::inverse(view) * eyeCoords;
		glm::vec3 rayDirection = glm::normalize(glm::vec3(worldCoords));

		// 假设相机位置为光线的起点
		glm::vec3 rayOrigin = mainCamera.getPosition();

		// 检测光线与球体是否相交
		struct Sphere {
			glm::vec3 center;
			float radius;
			std::string name;
		};
		Sphere spheres[] = {
			{ glm::vec3(-60.0f, 10.0f, 0.0f), 30.0f, "Sun" },
			{ glm::vec3(0.0f, 0.0f, 0.0f), 10.0f, "Earth" },
			{ glm::vec3(20.0f, 0.0f, -10.0f), 5.0f, "Moon" }
		};

		for (const auto& sphere : spheres) {
			glm::vec3 oc = rayOrigin - sphere.center;
			float a = glm::dot(rayDirection, rayDirection);
			float b = 2.0f * glm::dot(oc, rayDirection);
			float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
			float discriminant = b * b - 4 * a * c;

			if (discriminant > 0) {
				std::cout << "Clicked on: " << sphere.name << std::endl;
				break;
			}
		}
	}
}
