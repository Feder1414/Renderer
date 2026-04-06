//
// Created by USUARIO on 2/16/2026.
//

#ifndef GRAFICOS_OUTLINECOMPONENT_H
#define GRAFICOS_OUTLINECOMPONENT_H
#include <iostream>

#include "IComponent.h"
#include "vec3.hpp"

#include <format>

#include "Metadata/ComponentPropertiesMetadata.h"

class OutlineComponent : public IComponent, public ComponentPropertiesMetadata<OutlineComponent>
{
public:
    void SetThickNess(float thickness) { m_borderThickness = thickness; }
    void SetColor(const glm::vec3& color) { m_color = color; }
    float GetThickness() { return m_borderThickness; }
    glm::vec3 GetColor() { return m_color; }

    bool GetSeeTrough() const { return m_seeThrough; }
    void SetSeeTrough(bool seeTrough) { m_seeThrough = seeTrough; }


    void Update() override
    {
    };

    const std::string& GetComponentName() override
    {
        return GetMetadataComponentName();
    };

    const std::unordered_map<std::string, ComponentProperty>& GetComponentMetadata() override
    {
        return GetPropertiesMetadata();
    };

    void SetComponentMetadata() override
    {
        auto& alreadyInitialized = GetAlreadyInitialized();
        if (GetAlreadyInitialized())
        {
            return;
        }

        auto& componentMetadataName = GetMetadataComponentName();
        componentMetadataName = "Outline";
        auto& componentPropertiesMetadata = GetPropertiesMetadata();

        MAKE_PROPERTY("m_thickness", float, PropertyType::FLOAT, SetThickNess, GetThickness, "Outline", "")
        MAKE_PROPERTY("m_color", glm::vec3, PropertyType::COLOR, SetColor, GetColor, "Outline", "")
        alreadyInitialized = true;
    };

    size_t GetComponentId() const override
    {
        return GetComponentMetadataId();
    };

private:
    float m_borderThickness = 1.05f;
    glm::vec3 m_color = glm::vec3(0.8f, 0.0f, 0.0f);
    bool m_seeThrough = true;
};


#endif //GRAFICOS_OUTLINECOMPONENT_H
