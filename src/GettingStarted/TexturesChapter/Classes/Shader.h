//
// Created by USUARIO on 12/3/2025.
//

#ifndef GRAFICOS_SHADER_H
#define GRAFICOS_SHADER_H
#include <set>
#include <string>


class Shader
{
public:
    unsigned int m_shaderProgram, m_vertexShader, m_fragmentShader;


    Shader(const char* vertexPath, const char* fragmentPath);

    void Use() const;
    void setVariableBool(const std::string& variableName, bool value) const;
    void setVariableInt(const std::string& variableName, int value) const;
    void setVariableFloat(const std::string& variableName, float value) const;

private:
    std::string LoadShader(const char* shaderPath) const;
};

#endif //GRAFICOS_SHADER_H
