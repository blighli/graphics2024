#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>


void processInput(GLFWwindow* window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return 0;
	}
	glfwMakeContextCurrent(window);

	const char* vertexShaderSource = "#version 330 core\n\
									layout(location = 0) in vec3 pos;\n \
									void main()\n{\ngl_Position=vec4(pos, 1.0f);\n}\n;";
	const char* fragmentShaderSource = "#version 330 core\n\
									out vec4 frag;\n\
									void main()\n{\nfrag=vec4(1.0f, 0.5f, 0.2f, 1.0f);\n}\n";
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to init GLAD" << std::endl;
		return 0;
	}
	int vertexShader = glCreateShader(GL_VERTEX_SHADER); //����vs
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL); //shaderԴ�븽�ӵ�shader����
	glCompileShader(vertexShader); //����
	int success;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); //��ñ�����
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "error: vertexshader compilation failed  " << infoLog << std::endl;
		return 0;
	}
	int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER); //����fs
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	int shaderProgram = glCreateProgram(); //�����������
	glAttachShader(shaderProgram, vertexShader);   //shader���ӵ��������
	glAttachShader(shaderProgram, fragmentShader); //shader���ӵ��������
	glLinkProgram(shaderProgram); //��������
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success); //������ӽ��
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "error: programshader link failed   " << infoLog << std::endl;
		return 0;
	}
	glDeleteShader(vertexShader);   //������ϣ��Ѿ�û����
	glDeleteShader(fragmentShader);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window); //����
		glClearColor(0.5f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glUseProgram(shaderProgram);    //ʹ��
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		//someOpenGLFunctionThatDrawsOurTriangle();
		//glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		//glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window); //˫����Buffer
		glfwPollEvents(); //����¼����� �Ƿ������Ⱦ
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}