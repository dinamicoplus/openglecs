#pragma once

#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
	Shader();
	~Shader();

	void create(const char* vertexPath, const char* fragmentPath);
	void use() const;

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setFloat3(const std::string& name, float value1, float value2, float value3) const;
	void setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const;
	void setMatrix4(const std::string& name, const glm::mat4& value) const;

private:
	unsigned int m_shaderID;
};

