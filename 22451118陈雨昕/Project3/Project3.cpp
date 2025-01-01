#include "glew/glew.h"
#include "glfw/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Utils.h"
#include "ImportedModel.h"
#include "camera.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

struct Light {
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
};

std::vector<Light> lights;

void setupLights() {
	// 添加多个光源
	lights.push_back({ glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f });
	lights.push_back({ glm::vec3(-2.0f, 2.0f, 2.0f), glm::vec3(1.0f, 0.0f, 0.0f), 0.8f });
	lights.push_back({ glm::vec3(0.0f, 2.0f, -2.0f), glm::vec3(0.0f, 1.0f, 0.0f), 0.6f });
}

using namespace std;

static const int screen_width = 1920;
static const int screen_height = 1080;

static const int numVAOs = 1;
static const int numVBOs = 3;

static const float pai = 3.1415926f;

bool isModelCentered = true;  // 是否以模型为中心，默认为模型为中心
float modelScale = 1.0f;      // 模型缩放因子
float modelAngleX = 0.0f;     // 模型绕X轴旋转角度
float modelAngleY = 0.0f;     // 模型绕Y轴旋转角度


GLuint renderingProgram = 0;
GLuint vao[numVAOs] = { 0 };
GLuint vbo[numVBOs] = { 0 };

int width = 0;
int height = 0;
float aspect = 0.f;
float objLocX = 0.f, objLocY = 0.f, objLocZ = 0.f;
GLuint mvLoc = 0;
GLuint projLoc = 0;

GLuint shuttleTextureId = 0;

glm::mat4 mMat(1.f), vMat(1.f), pMat(1.f), mvMat(1.f);

Camera camera(glm::vec3(0.f, 0.f, 2.f));
float cameraX, cameraY, cameraZ;
ImportedModel myModel("resources/shuttle.obj");

GLboolean keys[1024] = { GL_FALSE };
GLboolean firstMouse = GL_TRUE;
float deltaTime = 0.f;
float lastFrame = 0.f;
float lastLocX = 0.f;
float lastLocY = 0.f;

float toRadians(float degrees)
{
	return (degrees * 2.f * pai) / 360.f;
}

void setupVertices(void)
{
	vector<glm::vec3> vert = myModel.getVertices();
	vector<glm::vec2> text = myModel.getTextureCoords();
	vector<glm::vec3> norm = myModel.getNormals();

	vector<float> pValues;
	vector<float> tValues;
	vector<float> nValues;

	for (int i = 0; i < myModel.getNumVertices(); i++)
	{
		/*pValues.push_back(vert[i * 3 + 0].x);
		pValues.push_back(vert[i * 3 + 1].y);
		pValues.push_back(vert[i * 3 + 2].z);

		tValues.push_back(text[i * 2 + 0].s);
		tValues.push_back(text[i * 2 + 1].t);

		nValues.push_back(norm[i * 3 + 0].x);
		nValues.push_back(norm[i * 3 + 1].y);
		nValues.push_back(norm[i * 3 + 2].z);*/

		pValues.push_back(vert[i].x);
		pValues.push_back(vert[i].y);
		pValues.push_back(vert[i].z);

		tValues.push_back(text[i].s);
		tValues.push_back(text[i].t);

		nValues.push_back(norm[i].x);
		nValues.push_back(norm[i].y);
		nValues.push_back(norm[i].z);
	}

	glGenVertexArrays(numVAOs, vao);
	glBindVertexArray(vao[0]);

	glGenBuffers(numVBOs, vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pValues.size() * sizeof(float), &(pValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getVertices().size() * sizeof(float), &(pVlaues[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tValues.size() * sizeof(float), &(tValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getTextureCoords().size() * sizeof(float), &(tValues[0]), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nValues.size() * sizeof(float), &(nValues[0]), GL_STATIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, myModel.getNormals().size() * sizeof(float), &(nValues[0]), GL_STATIC_DRAW);
}

void init(GLFWwindow* window)
{
	renderingProgram = Utils::createShaderProgram("shader/vertShader.glsl", "shader/fragShader.glsl");
	//glfwGetWindowSize(window, &width, &height);
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 2.f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(toRadians(45.f), aspect, 0.01f, 1000.f);

	setupVertices();
	setupLights();

	shuttleTextureId = Utils::loadTexture("resources/shuttle.jpg");

	/*lastLocX = (float)screen_width / 2.f;
	lastLocY = (float)screen_height / 2.f;*/
}


void window_size_callback(GLFWwindow* window, int newWidth, int newHeight)
{
	//屏幕坐标和窗口的帧缓冲
	/*GLFW在这里和这里解释文档中的两个坐标系。
		简而言之，窗口坐标是相对于监视器和 / 或窗口的，并且以不一定对应于真实屏幕像素的人造单元给出。 当DPI缩放被激活时（例如，在带有视网膜显示器的Mac上），情况尤其如此。
		与窗口坐标相比，帧缓冲区的大小与像素相关，以便与glViewport OpenGLs要求相匹配。
		请注意，在某些系统上，窗口坐标和像素坐标可以相同，但这不一定是正确的。*/
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);

	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 4.f;
	objLocX = 0.0f; objLocY = 0.0f; objLocZ = 0.0f;
	mMat = glm::translate(glm::mat4(1.f), glm::vec3(objLocX, objLocY, objLocZ));
	//pMat = glm::perspective(glm::radians(45.f), aspect, 0.001f, 1000.f);
	pMat = glm::perspective(camera.Zoom, aspect, 0.001f, 1000.f);
}

void do_movement(void)
{
	if (isModelCentered) {
		// 模型为中心模式
		if (keys[GLFW_KEY_W]) objLocY += 0.01f;  // 向上移动模型
		if (keys[GLFW_KEY_S]) objLocY -= 0.01f;  // 向下移动模型
		if (keys[GLFW_KEY_A]) objLocX -= 0.01f;  // 向左移动模型
		if (keys[GLFW_KEY_D]) objLocX += 0.01f;  // 向右移动模型
		if (keys[GLFW_KEY_Q]) modelAngleY += 0.01f; // 模型顺时针旋转
		if (keys[GLFW_KEY_E]) modelAngleY -= 0.01f; // 模型逆时针旋转
		if (keys[GLFW_KEY_UP]) modelScale += 0.01f;  // 放大模型
		if (keys[GLFW_KEY_DOWN]) modelScale -= 0.01f; // 缩小模型
	}
	else {
		// 相机漫游模式
		if (keys[GLFW_KEY_W]) camera.ProcessKeyboard(FORWARD, deltaTime);
		if (keys[GLFW_KEY_S]) camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (keys[GLFW_KEY_A]) camera.ProcessKeyboard(LEFT, deltaTime);
		if (keys[GLFW_KEY_D]) camera.ProcessKeyboard(RIGHT, deltaTime);
	}
}


void display(GLFWwindow* window, double currentTime) {
	// 清空颜色缓冲和深度缓冲
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.75f, 0.75f, 0.75f, 1.0f);

	// 使用着色器程序
	glUseProgram(renderingProgram);

	// 计算帧时间差
	deltaTime = currentTime - lastFrame;
	lastFrame = currentTime;

	// 检查按键输入，更新视点或模型的平移、旋转、缩放逻辑
	do_movement();

	// 获取 uniform 变量的位置
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	// 根据当前模式选择更新矩阵
	if (isModelCentered) {
		// 以模型为中心
		mMat = glm::mat4(1.0f); // 初始化模型矩阵
		mMat = glm::translate(mMat, glm::vec3(objLocX, objLocY, objLocZ)); // 模型平移
		mMat = glm::rotate(mMat, modelAngleX, glm::vec3(1.0f, 0.0f, 0.0f)); // 绕X轴旋转
		mMat = glm::rotate(mMat, modelAngleY, glm::vec3(0.0f, 1.0f, 0.0f)); // 绕Y轴旋转
		mMat = glm::scale(mMat, glm::vec3(modelScale, modelScale, modelScale)); // 模型缩放
		mvMat = camera.GetViewMatrix() * mMat; // 结合观察矩阵
	}
	else {
		// 以视点为中心
		mMat = glm::mat4(1.0f); // 初始化模型矩阵
		vMat = camera.GetViewMatrix(); // 更新视图矩阵
		mvMat = vMat * mMat; // 模型-视图矩阵
	}

	// 将模型-视图矩阵和投影矩阵传递到着色器
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	// 激活纹理单元并绑定纹理
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shuttleTextureId);

	// 传递光源数据到片段着色器
	int numLightsLoc = glGetUniformLocation(renderingProgram, "numLights");
	glUniform1i(numLightsLoc, lights.size());

	for (int i = 0; i < lights.size(); i++) {
		std::string lightBase = "lights[" + std::to_string(i) + "]";
		glUniform3fv(glGetUniformLocation(renderingProgram, (lightBase + ".position").c_str()), 1, glm::value_ptr(lights[i].position));
		glUniform3fv(glGetUniformLocation(renderingProgram, (lightBase + ".color").c_str()), 1, glm::value_ptr(lights[i].color));
		glUniform1f(glGetUniformLocation(renderingProgram, (lightBase + ".intensity").c_str()), lights[i].intensity);
	}

	// 传递观察者位置（相机位置）到片段着色器
	GLuint viewPosLoc = glGetUniformLocation(renderingProgram, "viewPos");
	glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera.Position));

	// 设置顶点属性指针（位置、纹理坐标、法线）
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// 开启深度测试
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	// 绘制模型
	glDrawArrays(GL_TRIANGLES, 0, myModel.getNumVertices());
}


void press_key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if ((key == GLFW_KEY_ESCAPE) && (action == GLFW_PRESS))
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
	if (action == GLFW_PRESS)
	{
		keys[key] = GLFW_TRUE;

		// 切换视点模式
		if (key == GLFW_KEY_TAB) {
			isModelCentered = !isModelCentered;
		}
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = GLFW_FALSE;
	}
}



void mouse_move_callback(GLFWwindow* window, double xPos, double yPos)
{
	if (firstMouse)
	{
		lastLocX = xPos;
		lastLocY = yPos;
		firstMouse = GL_FALSE;
	}

	double offsetLocX = xPos - lastLocX;
	double offsetLocY = lastLocY - yPos;

	lastLocX = xPos;
	lastLocY = yPos;

	camera.ProcessMouseMovement(offsetLocX, offsetLocY);
}

void mouse_scroll_callback(GLFWwindow* window, double xPos, double yPos)
{
	camera.ProcessMouseScroll(yPos);
}



int main(int argc, char** argv)
{
	int glfwState = glfwInit();
	if (GLFW_FALSE == glfwState)
	{
		cout << "GLFW initialize failed,invoke glfwInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screen_width, screen_height, "Load obj file model", nullptr, nullptr);
	if (!window)
	{
		cout << "GLFW create window failed,invoke glfwCreateWindow()......Error line:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, press_key_callback);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetScrollCallback(window, mouse_scroll_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);

	//设置鼠标模式
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	int glewState = glewInit();
	if (GLEW_OK != glewState)
	{
		cout << "GLEW initialize failed,invoke glewInit()......Error file:" << __FILE__ << "......Error line:" << __LINE__ << endl;
		glfwTerminate();
		exit(EXIT_FAILURE);
	}


	glfwSwapInterval(1);

	printf("%s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));//开始初始化过程
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	printf("GL Vendor : %s\n", vendor);
	printf("GL Renderer : %s\n", renderer);
	printf("GL Version (string) : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version : %s\n", glslVersion);

	glGetError(); // Debug GLEW bug fix

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	init(window);

	while (!glfwWindowShouldClose(window))
	{
		display(window, (float)glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);

	return 0;
}