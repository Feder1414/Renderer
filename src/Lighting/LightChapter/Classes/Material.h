//
// Created by USUARIO on 12/26/2025.
//

#ifndef GRAFICOS_MATERIAL_H
#define GRAFICOS_MATERIAL_H

#include "Texture.h"
#include <unordered_map>
#include <variant>
#include <vector>
#include <array>
#include <memory>

#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"


class Shader;

using UniformValue = std::variant<std::monostate, float, int, unsigned int, bool, glm::vec3, glm::mat4, Texture*,
                                  glm::mat3, std::shared_ptr<Texture>, std::array<float, 9>>;
using MaterialBasicProperty = std::variant<glm::vec3, Texture*>;


enum class MaterialPropertyEnum
{
    Ambient,
    Diffuse,
    Specular,
    Shininess,
    Transparency,
    HasSpecularTexture,
    SpecularMaterialSolid,
    Max
};

enum class TransparencyType
{
    Opaque,
    Semitransparent,
    Transparent,
};

class Material
{
    std::string m_materialKey;
    Shader* m_Shader = nullptr;
    std::vector<const Texture*> m_textures;
    std::unordered_map<
        std::string, UniformValue> uniformValues = {};
    std::array<UniformValue, static_cast<size_t>(MaterialPropertyEnum::Max)> m_materialProperties = {};
    TransparencyType m_transparencyType = TransparencyType::Opaque;

public:
    Material(const std::string& materialKey)
    {
        m_materialKey = materialKey;
        SetProperty(MaterialPropertyEnum::HasSpecularTexture, false);
    };
    void SetUniformValue(const std::string& variableName, const UniformValue& uniformValue);
    void SetProperty(MaterialPropertyEnum materialProperty, const UniformValue& propertyValue);
    void SetShader(Shader* shader) { m_Shader = shader; }
    static std::string MaterialPropertyNameToString(MaterialPropertyEnum materialProperty);
    const auto& GetMaterialProperties() const { return m_materialProperties; }

    const std::unordered_map<std::string, UniformValue>& GetShaderUniformValues() const { return uniformValues; }

    void SetTransparencyType(TransparencyType type)
    {
        m_transparencyType = type;
        SetProperty(MaterialPropertyEnum::Transparency, static_cast<int>(m_transparencyType));
    }

    TransparencyType GetTransparencyType() const { return m_transparencyType; }


    const std::string& GetMaterialKey() { return m_materialKey; }
    Shader* GetShader() const { return m_Shader; }
};


#endif //GRAFICOS_MATERIAL_H
