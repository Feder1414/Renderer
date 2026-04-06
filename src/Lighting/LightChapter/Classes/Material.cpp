//
// Created by USUARIO on 12/26/2025.
//

#include "Material.h"

#include <iostream>
#include <variant>
#include "Shader.h"


// Material::Material(Shader* shader) : m_Shader(shader)
// {
//     auto uniformNames = m_Shader->GetUniformNames();
//
//     for (auto uniformName : uniformNames)
//     {
//         uniformValues.insert({uniformName, UniformValue{std::monostate{}}});
//     }
// }

void Material::SetUniformValue(const std::string& variableName, const UniformValue& uniformValue)
{
    // auto it = uniformValues.find(variableName);
    //
    // if (it == uniformValues.end())
    // {
    //     std::cout << "Uniform variable with name " << variableName << " not found" << std::endl;
    //     return;
    // }
    // it->second = uniformValue;
    uniformValues[variableName] = uniformValue;
}


void Material::SetProperty(const MaterialPropertyEnum materialProperty, const UniformValue& propertyValue)
{
    auto index = static_cast<size_t>(materialProperty);
    m_materialProperties[index] = propertyValue;
}

std::string Material::MaterialPropertyNameToString(MaterialPropertyEnum materialProperty)
{
    switch (materialProperty)
    {
    case MaterialPropertyEnum::Ambient: return "materialAmbient";
    case MaterialPropertyEnum::Diffuse: return "materialDiffuse";
    case MaterialPropertyEnum::Specular: return "materialSpecular";
    case MaterialPropertyEnum::Shininess: return "materialShininess";
    case MaterialPropertyEnum::Transparency: return "transparency";
    case MaterialPropertyEnum::HasSpecularTexture: return "hasSpecularTexture";
    case MaterialPropertyEnum::SpecularMaterialSolid: return "specularMaterialSolid";
    default: return "Unknown property";
    }
}
