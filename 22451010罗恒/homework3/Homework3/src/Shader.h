#pragma once
#include <iostream>
class Shader
{
public:
	Shader(const std::string& verPath, const std::string& fragPath);
	template<typename T> 
	void setValue(const std::string& name, T value);
	void use();
private:
	uint32_t m_RenderID;
};
