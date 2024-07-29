#pragma once

#include <string>
#include <glad/glad.h>

class Shader
{
public:
	Shader();
	~Shader();

	void create(const char* vertexPath, const char* fragmentPath);
	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat3(const std::string& name, float value1, float value2, float value3) const;

private:
	unsigned int m_shaderID;
};

