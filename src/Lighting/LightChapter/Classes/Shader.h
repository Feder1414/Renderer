//
// Created by USUARIO on 12/3/2025.
//

#ifndef GRAFICOS_SHADER_H
#define GRAFICOS_SHADER_H
#include "glad/glad.h"
#include <any>
#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#include "fwd.hpp"
#include "Material.h"


struct ShaderResource
{
    std::string name;
    int size;
    GLenum type;
    bool isStruct;
    bool isArray;
    int nameLength;
    int location;
};

enum class ShaderBasicProperties
{
    ModelTransform,
    ViewTransform,
    ProjectionTransform,
    LightTransform,
    LightColor,
    LightPosition,
    NormalMatrix,
    EyePosition,
    GlobalAmbientLight,
    LightAmbientFactor,
    LightDiffuseFactor,
    LightSpecularFactor,
    Lights,
    AmountLights,
};


class Shader
{
public:
    unsigned int m_shaderProgram, m_vertexShader, m_fragmentShader;
    std::set<ShaderBasicProperties> m_activeBasicProperties;
    std::unordered_map<std::string, ShaderResource> m_uniformNameToResource;


    Shader(const char* vertexPath, const char* fragmentPath);
    void CreateShaderResources();
    const ShaderResource* GetShaderResource(const std::string& uniformName) const;
    std::vector<std::string> GetUniformNames() const;

    void Use() const;
    void setUniformPerName(const std::string& variableName, UniformValue& value);
    void setPrintDebug(bool debug) { m_printDebug = debug; }
    void setAmountPrintDebug(const int amountPrints) { m_amountDebugPrints = amountPrints; }
    static std::string ShaderPropertyEnumToStr(ShaderBasicProperties shaderProperty);
    const std::set<ShaderBasicProperties>& GetActiveProperties() const { return m_activeBasicProperties; }

private:
    bool m_printDebug = false;
    int m_amountDebugPrints = 1;
    int currAmountPrints = 0;
    std::function<void(std::any)> m_dynamicAttributesSetter;
    std::string LoadShader(const char* shaderPath) const;


    void setVariableBool(const std::string& variableName, bool value, int uniformLocation) const;
    void setVariableInt(const std::string& variableName, int value, int uniformLocation) const;
    void setVariableFloat(const std::string& variableName, float value, int uniformLocation) const;
    void setVariableMatrix4(const std::string& variableName, glm::mat4 value, int uniformLocation) const;
    void setVariableMatrix3(const std::string& variableName, glm::mat3 value, int uniformLocation);
    void setVariableVec3(const std::string& variableName, glm::vec3 value, int uniformLocation) const;
};

#endif //GRAFICOS_SHADER_H
