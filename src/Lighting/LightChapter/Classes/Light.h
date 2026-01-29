//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_LIGHT_H
#define GRAFICOS_LIGHT_H

#include "ModelRenderInfo.h"
#include "IComponent.h"

enum class LightType
{
    PointLight,
    DirectionalLight,
    SpotLight
};

enum class LightProperty
{
    Color,
    Direction,
    Position,
    DiffuseFactor,
    AmbientFactor,
    SpecularFactor,
    InnerConeAngle,
    OuterConeAngle,
    AttenuationConstant,
    AttenuationLinear,
    AttenuationQuadratic,
    LightType
};

class Light final : public IComponent
{
private:


public:
    LightType m_lightType = LightType::PointLight;
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 m_direction = glm::vec3(0.0f, -1.0f, 0.0f);
    float m_diffStrength = 0.5f;
    float m_ambientStrength = 0.1f;
    float m_specularStrength = 0.1f;
    float m_innerConeAngle = glm::radians(35.0f);
    float m_outerConeAngle = glm::radians(45.0f);
    float m_attenuationConstant = 1.0f;
    float m_attenuationLinear = 0.7f;
    float m_attenuationQuadratic = 1.8f;


    void Update() override
    {
    };

    static std::string LightPropertyEnumToStr(LightProperty lightProperty);
};


#endif //GRAFICOS_LIGHT_H
