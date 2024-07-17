#include "ShaderProgram.h"

#include <spdlog/spdlog.h>

#include <fstream>
#include <sstream>

GLuint ShaderProgram::m_ProgramID = 0;

void ShaderProgram::create(const std::string& vertexPath, const std::string& fragmentPath)
{
    // Compile shaders
    GLuint vertexShader = compileShader(readShaderCode(vertexPath), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(readShaderCode(fragmentPath), GL_FRAGMENT_SHADER);

    // Link both shaders into program

    m_ProgramID = glCreateProgram();
    glAttachShader(m_ProgramID, vertexShader);
    glAttachShader(m_ProgramID, fragmentShader);
    glLinkProgram(m_ProgramID);

    int  success;
    char infoLog[512];
    glGetProgramiv(m_ProgramID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_ProgramID, 512, NULL, infoLog);
        spdlog::error("Shader program linking failed: {}", infoLog);
    }

    glUseProgram(m_ProgramID);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void ShaderProgram::use()
{
    glUseProgram(m_ProgramID);
}

void ShaderProgram::destroy()
{
    glDeleteProgram(m_ProgramID);
}

std::string ShaderProgram::readShaderCode(const std::string& shaderPath)
{
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(shaderPath);
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
    }
    catch (std::ifstream::failure& e) {
        spdlog::error("Problem trying to read shader file from {}. Details: {}", shaderPath, e.what());
    }

    return shaderStream.str();
}

GLuint ShaderProgram::compileShader(const std::string& shaderCode, GLenum shaderType)
{
    const char* shaderCodeCStr = shaderCode.c_str();
    GLuint shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, &shaderCodeCStr, NULL);
    glCompileShader(shaderID);
    int  success;
    char infoLog[512];
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        spdlog::error("Shader compilation failed: {}", infoLog);
    }

    return shaderID;
}

