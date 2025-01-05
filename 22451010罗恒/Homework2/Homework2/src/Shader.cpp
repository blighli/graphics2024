#include "Shader.h"
#include <fstream>
#include <sstream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
Shader::Shader(const std::string& verPath, const std::string& fragPath)
{
	std::ifstream vertFile, FragFile;
	vertFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	FragFile.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	vertFile.open(verPath);
	FragFile.open(fragPath);

	std::stringstream verss, fragss;
	verss << vertFile.rdbuf();
	fragss << FragFile.rdbuf();

	std::string verstr, fragstr;
	verstr = verss.str();
	fragstr = fragss.str();

	const char *verc, *fragc;
	verc = verstr.c_str();
	fragc = fragstr.c_str();

	uint32_t vershader, fragshader;
	vershader = glCreateShader(GL_VERTEX_SHADER);
	fragshader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vershader, 1, &verc, nullptr);
	glShaderSource(fragshader, 1, &fragc, nullptr);
	int versuccess, fragsuccess;
	glCompileShader(vershader);
	glCompileShader(fragshader);
	glGetShaderiv(vershader, GL_COMPILE_STATUS, &versuccess);
	if (!versuccess)
	{
		std::cout << verPath << std::endl;
		int infoSize;
		glGetShaderiv(vershader, GL_INFO_LOG_LENGTH, &infoSize);
		std::vector<GLchar> info(infoSize);
		glGetShaderInfoLog(vershader, infoSize, &infoSize, info.data());
		std::cerr << info.data() << std::endl;
	}
	glGetShaderiv(fragshader, GL_COMPILE_STATUS, &fragsuccess);
	if (!fragsuccess)
	{
		std::cout << fragPath << std::endl;
		int infoSize;
		glGetShaderiv(fragshader, GL_INFO_LOG_LENGTH, &infoSize);
		std::vector<GLchar>info(infoSize);
		glGetShaderInfoLog(fragshader, infoSize, &infoSize, info.data());
		std::cerr << info.data() << std::endl;
	}
	m_RenderID = glCreateProgram();
	glAttachShader(m_RenderID, vershader);
	glAttachShader(m_RenderID, fragshader);
	glLinkProgram(m_RenderID);
	int programSuccess;
	glGetProgramiv(m_RenderID, GL_LINK_STATUS, &programSuccess);
	if (!programSuccess)
	{
		int infoSize;
		glGetProgramiv(m_RenderID, GL_INFO_LOG_LENGTH, &infoSize);
		std::vector<GLchar> info(infoSize);
		glGetProgramInfoLog(m_RenderID, infoSize, &infoSize, info.data());
		std::cerr << info.data() << std::endl;
	}
}

void Shader::use()
{
	glUseProgram(m_RenderID);
}

template<>
void Shader::setValue<glm::mat4>(const std::string& name, glm::mat4 value)
{
	glUniformMatrix4fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::setValue<int>(const std::string& name, int value)
{
	glUniform1i(glGetUniformLocation(m_RenderID, name.c_str()), value);
}

template<>
void Shader::setValue<float>(const std::string& name, float value)
{
	glUniform1f(glGetUniformLocation(m_RenderID, name.c_str()), value);
}

template<>
void Shader::setValue<glm::mat3>(const std::string& name, glm::mat3 value)
{
	glUniformMatrix3fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

template<>
void Shader::setValue<glm::vec3>(const std::string& name, glm::vec3 value)
{
	glUniform3fv(glGetUniformLocation(m_RenderID, name.c_str()), 1, glm::value_ptr(value));
}