#pragma once

#include <string>
#include <glad/glad.h>

class ShaderProgram
{
public:
	static void create(const std::string& vertexPath, const std::string& fragmentPath);

	static void use();
	static void destroy();

	GLuint getShaderProgram() { return m_ProgramID; }

private:
	static std::string readShaderCode(const std::string& shaderPath);
	static GLuint compileShader(const std::string& shaderCode, GLenum shaderType);
private:
	static GLuint m_ProgramID;
};

