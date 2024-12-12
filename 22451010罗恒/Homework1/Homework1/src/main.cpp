#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <fstream>
#include <sstream>
int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "first triangle", nullptr, nullptr);
	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "error create glad" << std::endl;
		return -1;
	}
	uint32_t shaderProgram;
	{
		uint32_t vershader, fragmentShader;
		std::fstream v_file, f_file;
		v_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		f_file.exceptions(std::ifstream::badbit | std::ifstream::failbit);
		v_file.open("src/shader/triangle.vs");
		f_file.open("src/shader/triangle.fs");
		std::stringstream vss, fss;
		vss << v_file.rdbuf();
		fss << f_file.rdbuf();
		std::string f_str, v_str;
		v_str = vss.str();
		f_str = fss.str();
		const GLchar* vsource;
		const GLchar* fsource;
		vsource = v_str.c_str();
		fsource = f_str.c_str();
		vershader = glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(vershader, 1, &vsource, nullptr);
		glShaderSource(fragmentShader, 1, &fsource, nullptr);

		glCompileShader(vershader);
		glCompileShader(fragmentShader);
		shaderProgram = glCreateProgram();
		glAttachShader(shaderProgram, vershader);
		glAttachShader(shaderProgram, fragmentShader);
		glLinkProgram(shaderProgram);
		{
			glDetachShader(shaderProgram, vershader);
			glDetachShader(shaderProgram, fragmentShader);
			glDeleteShader(vershader);
			glDeleteShader(fragmentShader);
		}
	}
	uint32_t triangleVAO;
	{
		glGenVertexArrays(1, &triangleVAO);
		uint32_t vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		float vertices[]{
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f
		};
		glBindVertexArray(triangleVAO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(sizeof(float) * 3));
		glBindVertexArray(0);
	}
	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(triangleVAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
	return 0;
}