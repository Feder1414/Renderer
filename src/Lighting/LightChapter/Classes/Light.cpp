//
// Created by USUARIO on 12/24/2025.
//

#include "Light.h"

#include "Shader.h"

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
