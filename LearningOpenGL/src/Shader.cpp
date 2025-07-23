#include "Shader.h"

#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>


Shader::Shader()
{
    
}

Shader::~Shader()
{
    glDeleteProgram(m_shaderID);
}

void Shader::create(const char* vertexPath, const char* fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        spdlog::error("ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        spdlog::error("ERROR::::SHADER::VERTEX::COMPILATION_FAILED \n{}", infoLog);

    };

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        spdlog::error("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED \n{}", infoLog);
    };

    // shader Program
    m_shaderID = glCreateProgram();
    glAttachShader(m_shaderID, vertex);
    glAttachShader(m_shaderID, fragment);
    glLinkProgram(m_shaderID);
    // print linking errors if any
    glGetProgramiv(m_shaderID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_shaderID, 512, NULL, infoLog);
        spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED \n{}", infoLog);
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void Shader::use() const
{
    glUseProgram(m_shaderID);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_shaderID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_shaderID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_shaderID, name.c_str()), value);
}

void Shader::setFloat3(const std::string& name, float value1, float value2, float value3) const
{
    glUniform3f(glGetUniformLocation(m_shaderID, name.c_str()), value1, value2, value3);
}

void Shader::setFloat4(const std::string& name, float value1, float value2, float value3, float value4) const
{
    glUniform4f(glGetUniformLocation(m_shaderID, name.c_str()), value1, value2, value3, value4);
}

void Shader::setMatrix4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_shaderID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_shaderID, name.c_str()), x, y, z);
}