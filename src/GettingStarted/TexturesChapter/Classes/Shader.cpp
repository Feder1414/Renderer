//
// Created by USUARIO on 12/3/2025.
//

#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    unsigned int vertexShader, fragmentShader;
    int success;
    char infoLog[512];

    m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
    auto vertexShaderSource = LoadShader(vertexPath);
    std::cout << "Vertex Shader Source" << vertexShaderSource << std::endl;
    auto vertexShaderCstr = vertexShaderSource.c_str();

    glShaderSource(m_vertexShader, 1, &vertexShaderCstr, NULL);
    glCompileShader(m_vertexShader);

    glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(m_vertexShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    auto fragmentShaderSource = LoadShader(fragmentPath);
    std::cout << "Fragment shader source" << fragmentShaderSource << std::endl;
    auto fragmentShaderCstr = fragmentShaderSource.c_str();
    glShaderSource(m_fragmentShader, 1, &fragmentShaderCstr, NULL);
    glCompileShader(m_fragmentShader);
    glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(m_fragmentShader, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }

    m_shaderProgram = glCreateProgram();
    glAttachShader(m_shaderProgram, m_vertexShader);
    glAttachShader(m_shaderProgram, m_fragmentShader);
    glLinkProgram(m_shaderProgram);

    glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
        std::cout << infoLog << std::endl;
    }
}


std::string Shader::LoadShader(const char* shaderPath) const
{
    std::ifstream shaderSource(shaderPath);

    if (!shaderSource.is_open())
    {
        std::cout << "File not found: " << shaderPath << std::endl;
    }

    try
    {
        std::stringstream shaderBuffer;
        shaderBuffer << shaderSource.rdbuf();
        return shaderBuffer.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }
    return "";
}

void Shader::Use() const
{
    glUseProgram(m_shaderProgram);
}

void Shader::setVariableBool(const std::string& variableName, bool value) const
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, variableName.c_str()), value);
}

void Shader::setVariableInt(const std::string& variableName, int value) const
{
    glUseProgram(m_shaderProgram);
    glUniform1i(glGetUniformLocation(m_shaderProgram, variableName.c_str()), value);
}

void Shader::setVariableFloat(const std::string& variableName, float value) const
{
    glUseProgram(m_shaderProgram);
    glUniform1f(glGetUniformLocation(m_shaderProgram, variableName.c_str()), value);
}
