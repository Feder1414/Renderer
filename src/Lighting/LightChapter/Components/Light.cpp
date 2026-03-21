//
// Created by USUARIO on 12/24/2025.
//

#include "Light.h"
#include <vector>
#include <memory>
#include "Shader.h"
#include "Metadata/GlobalEnumMetadata.h"
#include <format>
#include <iostream>

#include "Entity.h"

namespace
{
    std::unordered_map<LightProperty, std::string> lightPropertyEnumToStr = {
        {LightProperty::Color, "color"},
        {LightProperty::Position, "position"},
        {LightProperty::Direction, "direction"},
        {LightProperty::AmbientFactor, "ambientFactor"},
        {LightProperty::DiffuseFactor, "diffuseFactor"},
        {LightProperty::SpecularFactor, "specularFactor"},
        {LightProperty::InnerConeAngle, "innerConeAngle"},
        {LightProperty::OuterConeAngle, "outerConeAngle"},
        {LightProperty::AttenuationConstant, "attConstant"},
        {LightProperty::AttenuationLinear, "attLinear"},
        {LightProperty::AttenuationQuadratic, "attQuadratic"},
        {LightProperty::LightType, "type"}
    };
}

std::string Light::LightPropertyEnumToStr(LightProperty lightProperty)
{
    if (lightPropertyEnumToStr.contains(lightProperty))
    {
        return lightPropertyEnumToStr[lightProperty];
    }
    return "unknowProperty";
}

void Light::SetComponentMetadata()
{
    auto& componentMetadataName = GetMetadataComponentName();
    componentMetadataName = "Light";
    auto& componentPropertiesMetadata = GetPropertiesMetadata();

    REGISTER_ENUM(LIGHT_ENUM, "lightEnum");
    MAKE_PROPERTY("m_lightType", int, PropertyType::ENUM, SetLightTypeInt, GetLightTypeInt, "Light", "lightEnum");
    MAKE_PROPERTY("m_color", glm::vec3, PropertyType::VEC3, SetColor, GetColor, "Light", "")
    MAKE_PROPERTY("m_direction", glm::vec3, PropertyType::VEC3, SetDirection, GetDirection, "Light", "")
    MAKE_PROPERTY("m_diffStrength", float, PropertyType::FLOAT, SetDiffStrength, GetDiffStrength, "Light", "")
    MAKE_PROPERTY("m_ambientStrength", float, PropertyType::FLOAT, SetAmbientStrength, GetAmbientStrength, "Light", "")
    MAKE_PROPERTY("m_specularStrength", float, PropertyType::FLOAT, SetSpecularStrength, GetSpecularStrength, "Light",
                  "")
    MAKE_PROPERTY("m_attenuationConstant", float, PropertyType::FLOAT, SetAttenuationConstant, GetAttenuationConstant,
                  "Light", "")
    MAKE_PROPERTY("m_attenuationLinear", float, PropertyType::FLOAT, SetAttenuationLinear, GetAttenuationLinear,
                  "Light", "")
    MAKE_PROPERTY("m_attenuationQuadratic", float, PropertyType::FLOAT, SetAttenuationQuadratic,
                  GetAttenuationQuadratic, "Light", "")
    MAKE_PROPERTY("m_innerConeAngle", float, PropertyType::FLOAT, SetInnerConeAngle, GetInnerConeAngle, "Light", "")
    MAKE_PROPERTY("m_outerConeAngle", float, PropertyType::FLOAT, SetOuterConeAngle, GetOuterConeAngle, "Light", "")
}

LightGPU Light::GetLightGPU() const
{
    return {
        .color = m_color, .direction = m_direction, .position = entity->GetWorldPos(), .diffuseFactor = m_diffStrength,
        .ambientFactor = m_ambientStrength,
        .specularFactor = m_specularStrength, .innerConeAngle = m_innerConeAngle, .outerConeAngle = m_outerConeAngle,
        .attConstant = m_attenuationConstant, .attLinear = m_attenuationLinear, .attQuadratic = m_attenuationQuadratic,
        .type = static_cast<int>(m_lightType)
    };
}
