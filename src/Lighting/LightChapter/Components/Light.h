//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_LIGHT_H
#define GRAFICOS_LIGHT_H

#include "ModelRenderInfo.h"
#include "IComponent.h"
#include "Metadata/ComponentPropertiesMetadata.h"

#define LIGHT_ENUM(X) \
    X(PointLight, 0), \
    X(DirectionalLight, 1), \
    X(SpotLight, 2)


#define X(name, value) name

enum class LightType
{
    LIGHT_ENUM(X)
};
#undef X

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

class Light final : public IComponent, public ComponentPropertiesMetadata<Light>
{
public:
    LightType m_lightType = LightType::PointLight;
    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 m_direction = glm::vec3(0.0f, -1.0f, 0.0f);
    float m_diffStrength = 0.5f;
    float m_ambientStrength = 0.1f;
    float m_specularStrength = 0.1f;

    float m_attenuationConstant = 1.0f;
    float m_attenuationLinear = 0.09f;
    float m_attenuationQuadratic = 0.032f;


    void SetInnerConeAngle(float gradeAngle) { m_innerConeAngle = cos(glm::radians(gradeAngle)); }
    void SetOuterConeAngle(float gradeAngle) { m_outerConeAngle = cos(glm::radians(gradeAngle)); }
    float GetInnerConeAngle() const { return m_innerConeAngle; }
    float GetOuterConeAngle() const { return m_outerConeAngle; }

    glm::vec3 GetColor() { return m_color; }
    void SetColor(const glm::vec3& color) { m_color = color; }

    void SetDirection(const glm::vec3& direction) { m_direction = direction; }
    glm::vec3 GetDirection() const { return m_direction; }

    float GetDiffStrength() const { return m_diffStrength; }
    void SetDiffStrength(float diffStrength) { m_diffStrength = diffStrength; }
    float GetAmbientStrength() const { return m_ambientStrength; }
    void SetAmbientStrength(float ambientStrength) { m_ambientStrength = ambientStrength; }
    float GetSpecularStrength() const { return m_specularStrength; }
    void SetSpecularStrength(float specularStrength) { m_specularStrength = specularStrength; }

    float GetAttenuationConstant() const { return m_attenuationConstant; }
    void SetAttenuationConstant(float attConstant) { m_attenuationConstant = attConstant; }
    float GetAttenuationLinear() const { return m_attenuationLinear; }
    void SetAttenuationLinear(float attLinear) { m_attenuationLinear = attLinear; }

    void SetAttenuationQuadratic(float attenuationQuadratic) { m_attenuationQuadratic = attenuationQuadratic; }
    float GetAttenuationQuadratic() const { return m_attenuationQuadratic; }

    void SetLightTypeInt(int type) { m_lightType = static_cast<LightType>(type); }
    int GetLightTypeInt() { return static_cast<int>(m_lightType); }


    static std::string LightPropertyEnumToStr(LightProperty lightProperty);

    const std::string& GetComponentName() override
    {
        static std::string name = "Light";
        return name;
    }

    const std::unordered_map<std::string, ComponentProperty>& GetComponentMetadata() override
    {
        return GetPropertiesMetadata();
    }

    void SetComponentMetadata() override;

private:
    float m_innerConeAngle = glm::radians(35.0f);
    float m_outerConeAngle = glm::radians(45.0f);


    void Update() override
    {
    };
};


#endif //GRAFICOS_LIGHT_H
