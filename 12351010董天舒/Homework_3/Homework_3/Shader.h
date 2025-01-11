// Shader.h
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;

    // 构造函数读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);

    // 使用/激活着色器
    void use();

    // 设置uniform函数
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
};

#endif
