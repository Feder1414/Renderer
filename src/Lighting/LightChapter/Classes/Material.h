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

using UniformValue = std::variant<float, int, unsigned int, bool, std::monostate, glm::vec3, glm::mat4, Texture*,
                                  glm::mat3, std::shared_ptr<Texture>>;
using MaterialBasicProperty = std::variant<glm::vec3, Texture*>;


enum class MaterialPropertyEnum
{
    Ambient,
    Diffuse,
    Specular,
    Shininess,
    Max
};

class Material
{
    Shader* m_Shader = nullptr;
    std::vector<const Texture*> m_textures;
    std::unordered_map<
        std::string, UniformValue> uniformValues = {};
    std::array<UniformValue, static_cast<size_t>(MaterialPropertyEnum::Max)> m_materialProperties = {};

public:
    void SetUniformValue(const std::string& variableName, const UniformValue& uniformValue);
    void SetProperty(MaterialPropertyEnum materialProperty, UniformValue propertyValue);
    void SetShader(Shader* shader) { m_Shader = shader; }
    static std::string MaterialPropertyNameToString(MaterialPropertyEnum materialProperty);
    const auto& GetMaterialProperties() const { return m_materialProperties; }

    const std::unordered_map<std::string, UniformValue>& GetShaderUniformValues() const { return uniformValues; }

    Shader* GetShader() const { return m_Shader; }
};


#endif //GRAFICOS_MATERIAL_H
