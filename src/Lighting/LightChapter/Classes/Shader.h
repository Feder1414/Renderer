//
// Created by USUARIO on 12/3/2025.
//

#ifndef GRAFICOS_SHADER_H
#define GRAFICOS_SHADER_H
#include "glad/glad.h"
#include <any>
#include <functional>
#include <optional>
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

struct ShaderSource
{
    std::string source;
    // True if is source code, false if it is a path to a file
    bool isSourceCode = false;
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
    BorderColor,
    FogColor,
    FogDensity,
    FogEffect,
    NearPlane,
    FarPlane,
    BBCenter,
    AABBExtents,
    RenderedScene,
    ApplyKernel,
    Kernel,
    RenderedSceneWidth,
    RenderedSceneHeight,
    SkyBox,
    UsingBlin,
    ShadowMap
};


class Shader
{
public:
    unsigned int m_shaderProgram, m_vertexShader, m_fragmentShader, m_geoShader;
    std::set<ShaderBasicProperties> m_activeBasicProperties = {};
    std::unordered_map<std::string, ShaderResource> m_uniformNameToResource;
    std::string m_vertexShaderPath, m_fragmentShaderPath;
    std::string m_geoPath = "Not defined";


    Shader(const char* vertexPath, const char* fragmentPath, const char* geoPath = nullptr);
    Shader(const ShaderSource& vertexSource, const ShaderSource& fragmentSource,
           std::optional<ShaderSource> geoSource = std::nullopt);
    void SetShaderKey(const std::string& shaderKey) { m_shaderKey = shaderKey; }
    std::string GetShaderSourceCode(const ShaderSource& shaderSource, std::string& memberPath);
    void CreateShaderResources();
    const ShaderResource* GetShaderResource(const std::string& uniformName) const;
    std::vector<std::string> GetUniformNames() const;

    void Use() const;
    void setUniformPerName(const std::string& variableName, const UniformValue& value);
    void setUniformPerName(ShaderBasicProperties shaderBasicProperty, const UniformValue& uniformValue);
    void setPrintDebug(bool debug) { m_printDebug = debug; }
    void setAmountPrintDebug(const int amountPrints) { m_amountDebugPrints = amountPrints; }
    static std::string ShaderPropertyEnumToStr(ShaderBasicProperties shaderProperty);
    const std::string& GetShaderKey() const { return m_shaderKey; }
    const std::set<ShaderBasicProperties>& GetActiveProperties() const { return m_activeBasicProperties; }

private:
    bool m_printDebug = false;
    int m_amountDebugPrints = 1;
    int currAmountPrints = 0;
    std::string m_shaderKey;
    std::function<void(std::any)> m_dynamicAttributesSetter;
    std::string LoadShader(const char* shaderPath) const;

    void CompileShader(unsigned int& shaderId, const std::string& shaderPath, GLenum shaderType);
    void setVariableBool(const std::string& variableName, bool value, int uniformLocation) const;
    void setVariableInt(const std::string& variableName, int value, int uniformLocation) const;
    void setVariableFloat(const std::string& variableName, float value, int uniformLocation) const;
    void setVariableMatrix4(const std::string& variableName, glm::mat4 value, int uniformLocation) const;
    void setVariableMatrix3(const std::string& variableName, glm::mat3 value, int uniformLocation);
    void setVariableFloatArray(const std::string& variableName, const float* value, int count, int uniformLocation);
    void setVariableVec3(const std::string& variableName, glm::vec3 value, int uniformLocation) const;
};

#endif //GRAFICOS_SHADER_H
