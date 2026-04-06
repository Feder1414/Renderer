//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_LIGHT_H
#define GRAFICOS_LIGHT_H

#include "Entity.h"
#include "Frustum.h"
#include "ModelRenderInfo.h"
#include "IComponent.h"
#include "Metadata/ComponentPropertiesMetadata.h"

#include "glm.hpp"

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


struct LightGPU
{
    glm::vec3 color;
    float pad;

    glm::vec3 direction;
    float pad1;

    glm::vec3 position;

    float diffuseFactor;
    float ambientFactor;
    float specularFactor;

    float innerConeAngle;
    float outerConeAngle;

    float attConstant;
    float attLinear;
    float attQuadratic;
    int type;
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

    glm::mat4 m_projMat;
    glm::mat4 m_viewMat;


    void SetInnerConeAngle(float gradeAngle) { m_innerConeAngle = cos(glm::radians(gradeAngle)); }
    void SetOuterConeAngle(float gradeAngle) { m_outerConeAngle = cos(glm::radians(gradeAngle)); }
    float GetInnerConeAngle() const { return m_innerConeAngle; }
    float GetOuterConeAngle() const { return m_outerConeAngle; }

    bool GetCastShadows() const { return m_castShadows; }
    void SetCastShadows(bool castShadows) { m_castShadows = castShadows; }

    glm::vec3 GetColor() { return m_color; }
    void SetColor(const glm::vec3& color) { m_color = color; }

    void SetDirection(const glm::vec3& direction) { m_direction = direction; }
    glm::vec3 GetDirection() const;

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

    const glm::mat4& GetOrthoProjMatFromLightFrustum(glm::mat4 viewProjCam, bool recalculateBonds = true);

    void SetLightTypeInt(int type) { m_lightType = static_cast<LightType>(type); }
    int GetLightTypeInt() { return static_cast<int>(m_lightType); }

    LightType GetLightType() const { return m_lightType; }

    const glm::mat4& GetViewMat();

    const glm::mat4& GetOrthoProjMat();


    static std::string LightPropertyEnumToStr(LightProperty lightProperty);

    const std::string& GetComponentName() override
    {
        static std::string name = "Light";
        return name;
    }

    void SetFrustumAdditionalExtent(float extent) { m_frustumAdditionalExtent = extent; }

    void CalculateFrustum();

    const std::unordered_map<std::string, ComponentProperty>& GetComponentMetadata() override
    {
        return GetPropertiesMetadata();
    }

    void SetComponentMetadata() override;

    [[nodiscard]] LightGPU GetLightGPU() const;

    bool IsInViewFrustum(AABB* worldAABB) const;

    size_t GetComponentId() const override
    {
        return GetComponentMetadataId();
    };

private:
    float m_innerConeAngle = glm::radians(35.0f);
    float m_outerConeAngle = glm::radians(45.0f);
    float m_near = 0.1f;
    float m_far = 20.0f;
    float m_left = 20.0f;
    float m_right = 20.0f;
    float m_frustumAdditionalExtent = -0.0f;
    float m_top = 20.0f;
    float m_bottom = 20.0f;
    bool m_castShadows = false;

    bool m_dirtyRecalculateViewMat = true;

    bool visualizeFrustum = false;

    glm::vec3 prevEntityPos = glm::vec3(0.0f);


    Frustum m_frustum;


    void Update() override
    {
        if (prevEntityPos != entity->GetLocalPos())
        {
            m_dirtyRecalculateViewMat = true;
        }
    };
};


#endif //GRAFICOS_LIGHT_H
