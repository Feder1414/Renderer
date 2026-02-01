//
// Created by USUARIO on 12/3/2025.
//

#include "Shader.h"

#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <glad/glad.h>

#include "Material.h"


namespace
{
    std::unordered_map<std::string, ShaderBasicProperties> StringToShaderPropertiesEnum =
    {
        {
            "modelTransform", ShaderBasicProperties::ModelTransform
        },
        {
            "viewTransform", ShaderBasicProperties::ViewTransform
        },
        {
            "projectionTransform", ShaderBasicProperties::ProjectionTransform
        },
        {
            "lightTransform", ShaderBasicProperties::LightTransform
        },
        {"lightColor", ShaderBasicProperties::LightColor},
        {"lightPosition", ShaderBasicProperties::LightPosition},
        {"normalMatrix", ShaderBasicProperties::NormalMatrix},
        {"eyePosition", ShaderBasicProperties::EyePosition},
        {"globalAmbientLight", ShaderBasicProperties::GlobalAmbientLight},
        {"lightAmbientFactor", ShaderBasicProperties::LightAmbientFactor},
        {"lightDiffuseFactor", ShaderBasicProperties::LightDiffuseFactor},
        {"lightSpecularFactor", ShaderBasicProperties::LightSpecularFactor},
        {"lights", ShaderBasicProperties::Lights},
        {"amountLights", ShaderBasicProperties::AmountLights}


    };
}


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
    CreateShaderResources();
}

void Shader::CreateShaderResources()
{
    GLint amountUniforms;

    glGetProgramiv(m_shaderProgram, GL_ACTIVE_UNIFORMS, &amountUniforms);

    for (int i = 0; i < amountUniforms; i++)
    {
        const GLsizei bufSize = 256;
        GLchar name[bufSize];
        ShaderResource shaderResource = ShaderResource();


        glGetActiveUniform(m_shaderProgram, static_cast<GLuint>(i), 256, &shaderResource.nameLength,
                           &shaderResource.size, &shaderResource.type, name);


        auto uniformNameOriginal = std::string(name, shaderResource.nameLength);

        auto uniformLocation = glGetUniformLocation(m_shaderProgram, uniformNameOriginal.c_str());
        shaderResource.location = uniformLocation;

        auto uniformNameProccesed = uniformNameOriginal;

        auto dotIndex = uniformNameOriginal.find(".");
        auto startBracketIndex = uniformNameOriginal.find("[");


        if (dotIndex != std::string::npos)
        {
            shaderResource.isStruct = true;
            uniformNameProccesed = uniformNameOriginal.substr(0, dotIndex);
        }

        if ((startBracketIndex != std::string::npos) || (startBracketIndex != std::string::npos && dotIndex !=
            std::string::npos))
        {
            shaderResource.isArray = true;
            uniformNameProccesed = uniformNameOriginal.substr(0, startBracketIndex);
        }


        shaderResource.name = uniformNameOriginal;

        auto key = uniformNameOriginal;

        std::string stringInsertedShaderResource = std::format("Created Shader Resource with name {} and type {}",
                                                               key, shaderResource.type);

        m_uniformNameToResource.insert({key, std::move(shaderResource)});


        std::cout << stringInsertedShaderResource << std::endl;

        if (StringToShaderPropertiesEnum.contains(key))
        {
            std::cout << "The shader program has the basic property " << ShaderPropertyEnumToStr(
                StringToShaderPropertiesEnum.at(key)) << std::endl;
            m_activeBasicProperties.insert(StringToShaderPropertiesEnum[key]);
        }
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

const ShaderResource* Shader::GetShaderResource(const std::string& uniformName) const
{
    auto it = m_uniformNameToResource.find(uniformName);
    if (it == m_uniformNameToResource.end())
    {
        std::cout << "Uniform with name: " << uniformName << " not found" << std::endl;
        return nullptr;
    }
    return &it->second;
}

std::vector<std::string> Shader::GetUniformNames() const
{
    std::vector<std::string> uniformNames(m_uniformNameToResource.size());
    for (const auto& pair : m_uniformNameToResource)
    {
        uniformNames.push_back(pair.first);
    }
    return uniformNames;
}


void Shader::Use() const
{
    glUseProgram(m_shaderProgram);
}

void Shader::setVariableBool(const std::string& variableName, bool value, int uniformLocation) const
{
    glUniform1i(uniformLocation, value);
}

void Shader::setVariableInt(const std::string& variableName, int value, int uniformLocation) const
{
    glUniform1i(uniformLocation, value);
}

void Shader::setVariableFloat(const std::string& variableName, float value, int uniformLocation) const
{
    glUniform1f(uniformLocation, value);
}

void Shader::setVariableMatrix4(const std::string& variableName, glm::mat4 value, int uniformLocation) const
{

    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVariableMatrix3(const std::string& variableName, glm::mat3 value, int uniformLocation)
{

    glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(value));
}


void Shader::setVariableVec3(const std::string& variableName, glm::vec3 value, int uniformLocation) const
{
    glUniform3fv(uniformLocation, 1, glm::value_ptr(value));
}

void Shader::setUniformPerName(const std::string& variableName, UniformValue& uniformValue)
{
    auto it = m_uniformNameToResource.find(variableName);
    if (it == m_uniformNameToResource.end())
    {
        if (m_printDebug && currAmountPrints < m_amountDebugPrints)
        {
            std::cout << "The uniform with the name " << variableName << " does not exist" << std::endl;
            currAmountPrints += 1;
        }

        return;
    }
    auto& shaderResource = it->second;

    std::visit([&shaderResource, this](auto&& x)
    {
        auto uniformName = shaderResource.name;

        using T = std::decay_t<decltype(x)>;

        if constexpr (std::is_same_v<T, std::monostate>)
        {
            //Nada
        }
        else if constexpr (std::is_same_v<T, float>)
        {
            this->setVariableFloat(uniformName, x, shaderResource.location);
        }
        else if constexpr (std::is_same_v<T, int>)
        {
            this->setVariableInt(uniformName, x, shaderResource.location);
        }
        else if constexpr (std::is_same_v<T, unsigned int>)
        {
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
        }
        else if constexpr (std::is_same_v<T, glm::vec3>)
        {
            this->setVariableVec3(uniformName, x, shaderResource.location);
        }
        else if constexpr (std::is_same_v<T, glm::mat4>)
        {
            this->setVariableMatrix4(uniformName, x, shaderResource.location);
        }
        else if constexpr (std::is_same_v<T, glm::mat3>)
        {
            this->setVariableMatrix3(uniformName, x, shaderResource.location);
        }
        else if constexpr (std::is_same_v<T, Texture*>)
        {
            x->BindTexture();
            this->setVariableInt(uniformName, x->GetTextureSlot(), shaderResource.location);
        }
    }, uniformValue);
}

std::string Shader::ShaderPropertyEnumToStr(ShaderBasicProperties shaderProperty)
{
    if (shaderProperty == ShaderBasicProperties::LightTransform)
    {
        return "lightTransform";
    }
    if (shaderProperty == ShaderBasicProperties::ModelTransform)
    {
        return "modelTransform";
    }
    if (shaderProperty == ShaderBasicProperties::ViewTransform)
    {
        return "viewTransform";
    }
    if (shaderProperty == ShaderBasicProperties::ProjectionTransform)
    {
        return "projectionTransform";
    }
    if (shaderProperty == ShaderBasicProperties::LightColor)
    {
        return "lightColor";
    }
    if (shaderProperty == ShaderBasicProperties::LightPosition)
    {
        return "lightPosition";
    }
    if (shaderProperty == ShaderBasicProperties::NormalMatrix)
    {
        return "normalMatrix";
    }
    if (shaderProperty == ShaderBasicProperties::EyePosition)
    {
        return "eyePosition";
    }
    if (shaderProperty == ShaderBasicProperties::GlobalAmbientLight)
    {
        return "globalAmbientLight";
    }
    if (shaderProperty == ShaderBasicProperties::LightAmbientFactor)
    {
        return "lightAmbientFactor";
    }
    if (shaderProperty == ShaderBasicProperties::LightSpecularFactor)
    {
        return "lightSpecularFactor";
    }
    if (shaderProperty == ShaderBasicProperties::LightDiffuseFactor)
    {
        return "lightDiffuseFactor";
    }
    if (shaderProperty == ShaderBasicProperties::Lights)
    {
        return "lights";
    }
    if (shaderProperty == ShaderBasicProperties::AmountLights)
    {
        return "amountLights";
    }
    return "Unkown property";
}
