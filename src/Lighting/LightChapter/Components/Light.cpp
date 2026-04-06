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


#include <limits>
#include "Entity.h"

#include <glm.hpp>

namespace
{
    std::array<glm::vec3, 8> frustumNDCCoordinates = {
        //Front
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, 1.0f),
        //Back
        glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f)
    };
}

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

glm::vec3 Light::GetDirection() const
{
    return entity->GetForward();
}

const glm::mat4& Light::GetViewMat()
{
    m_viewMat = glm::lookAt(entity->GetWorldPos(), entity->GetWorldPos() + entity->GetForward(), entity->GetUp());
    m_dirtyRecalculateViewMat = false;


    return m_viewMat;
}

const glm::mat4& Light::GetOrthoProjMat()
{
    m_projMat = glm::ortho(-20.0f, 20.0f, -20.f, 20.0f, m_near, -m_far);
    return m_projMat;
}

const glm::mat4& Light::GetOrthoProjMatFromLightFrustum(glm::mat4 viewProjCam, bool recalculateBonds)
{
    //std::array<glm::vec3, 8> frustumLightCoordinates = {};
    if (recalculateBonds)
    {
        auto lightViewMat = GetViewMat();
        auto invViewProj = glm::inverse(viewProjCam);
        float maxX = std::numeric_limits<float>::lowest();
        float minX = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::lowest();;
        float minY = std::numeric_limits<float>::max();;
        float maxZ = std::numeric_limits<float>::lowest();;
        float minZ = std::numeric_limits<float>::max();


        for (int i = 0; i < frustumNDCCoordinates.size(); i++)
        {
            // Pass to world coordinates
            auto worldCoordinatesFrustumPoint = invViewProj * glm::vec4(frustumNDCCoordinates[i], 1.0f);
            worldCoordinatesFrustumPoint = worldCoordinatesFrustumPoint / worldCoordinatesFrustumPoint.w;
            auto lightSpaceFrustumPoint = glm::vec3(lightViewMat * worldCoordinatesFrustumPoint);

            maxX = std::max(maxX, lightSpaceFrustumPoint.x);
            minX = std::min(minX, lightSpaceFrustumPoint.x);
            maxY = std::max(maxY, lightSpaceFrustumPoint.y);
            minY = std::min(minY, lightSpaceFrustumPoint.y);
            maxZ = std::max(maxZ, lightSpaceFrustumPoint.z);
            minZ = std::min(minZ, lightSpaceFrustumPoint.z);
        }

        m_right = maxX + m_frustumAdditionalExtent;
        m_left = minX - m_frustumAdditionalExtent;
        m_top = maxY + m_frustumAdditionalExtent;
        m_bottom = minY - m_frustumAdditionalExtent;
        m_far = maxZ + m_frustumAdditionalExtent;
        m_near = minZ - m_frustumAdditionalExtent;
        m_projMat = glm::ortho(m_left, m_right, m_bottom, m_top, m_near, m_far);


    }
    return m_projMat;
}


std::string Light::LightPropertyEnumToStr(LightProperty lightProperty)
{
    if (lightPropertyEnumToStr.contains(lightProperty))
    {
        return lightPropertyEnumToStr[lightProperty];
    }
    return "unknowProperty";
}

void Light::CalculateFrustum()
{
    m_frustum = Frustum(m_viewMat, m_projMat);
}

bool Light::IsInViewFrustum(AABB* worldAABB) const
{
    return worldAABB->IsOnFrustum(m_frustum);
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
    MAKE_PROPERTY("castShadows", bool, PropertyType::BOOL, SetCastShadows, GetCastShadows, "Light", "")
}

LightGPU Light::GetLightGPU() const
{
    return {
        .color = m_color, .direction = entity->GetForward(), .position = entity->GetWorldPos(),
        .diffuseFactor = m_diffStrength,
        .ambientFactor = m_ambientStrength,
        .specularFactor = m_specularStrength, .innerConeAngle = m_innerConeAngle, .outerConeAngle = m_outerConeAngle,
        .attConstant = m_attenuationConstant, .attLinear = m_attenuationLinear, .attQuadratic = m_attenuationQuadratic,
        .type = static_cast<int>(m_lightType)
    };
}
