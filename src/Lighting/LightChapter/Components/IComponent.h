//
// Created by USUARIO on 1/24/2026.
//

#ifndef GRAFICOS_COMPONENT_H
#define GRAFICOS_COMPONENT_H

#include <unordered_map>
#include <string>
#include "Metadata/ComponentProperty.h"

#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)


#define MAKE_PROPERTY(propertyName, cType, enumType, setterName,getterName, componentName, enumName) \
        componentPropertiesMetadata.emplace(propertyName, ComponentProperty(propertyName, enumType,enumName )); \
        m_propertySetterGetter.emplace( \
            propertyName, \
            ComponentPropertySetterGetter( \
                [this](const std::any& value) \
                { \
                    try \
                    { \
                        auto valueC = std::any_cast<cType>(value); \
                        setterName(valueC); \
                    } \
                    catch (const std::bad_any_cast&) \
                    { \
                        std::cerr << std::format( \
                        Components::setterError, \
                        propertyName, \
                        componentName \
                        ) << std::endl; \
                    } \
                }, \
                [this]() -> std::any \
                { \
                    return std::any{ getterName() }; \
                } \
            ) \
        );
#define VALUE_ENUM(name, value) value
#define NAME_ENUM(name, value) #name
#define REGISTER_ENUM(ENUM_MACRO, enumName)\
    do{ \
        std::vector<int> values = {ENUM_MACRO(VALUE_ENUM)}; \
        std::vector<std::string> names = {ENUM_MACRO(NAME_ENUM)}; \
        auto enumMetadata = std::make_unique<EnumMetadata>(names, values); \
        GlobalEnumMetadata::AddEnumMetadata(enumName, std::move(enumMetadata)); \
    } while (0)


class Entity;

namespace Components
{
    inline constexpr std::string_view setterError = "Property {} of the component {} could not be set";
}

class IComponent
{
protected:
    Entity* entity = nullptr;
    std::unordered_map<std::string, ComponentPropertySetterGetter> m_propertySetterGetter;
    std::string m_componentName = "Default";

public:
    virtual ~IComponent() = default;
    virtual void Update() = 0;
    void SetEntity(Entity* entityP) { entity = entityP; }

    virtual const std::unordered_map<std::string, ComponentProperty>& GetComponentMetadata()
    {
        return {};
    }

    const auto& GetSetterGetter()
    {
        return m_propertySetterGetter;
    }

    virtual const std::string& GetComponentName() = 0;


    virtual void SetComponentMetadata()
    {
    };
    ///virtual std::unique_ptr<IComponent> Clone() = 0;
    Entity* GetEntity() const { return entity; }
};


#endif //GRAFICOS_COMPONENT_H
