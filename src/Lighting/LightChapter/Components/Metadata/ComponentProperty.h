//
// Created by USUARIO on 2/16/2026.
//

#ifndef GRAFICOS_COMPONENTPROPERTY_H
#define GRAFICOS_COMPONENTPROPERTY_H
#include <any>
#include <functional>
#include <string>

enum class PropertyType
{
    FLOAT,
    INT,
    BOOL,
    DOUBLE,
    ENUM,
    VEC3,
    COLOR,
    NONE
};

// Asumes lambda or something that captures the object
struct ComponentPropertySetterGetter
{
    ComponentPropertySetterGetter(const std::function<void(const std::any& value)>& setter,
                                  const std::function<std::any()>& getter) : m_setter(setter), m_getter(getter)
    {
    }

    const std::function<void(const std::any& value)>& GetSetter() const
    {
        return m_setter;
    }

    const std::function<std::any()>& GetGetter() const
    {
        return m_getter;
    }

private:
    const std::function<void(const std::any& value)> m_setter;
    const std::function<std::any()> m_getter;
};

class EnumMetadata
{
    std::vector<std::string> m_names;
    std::vector<int> m_values;

public:
    EnumMetadata(const std::vector<std::string>& names, std::vector<int> values) : m_names(names), m_values(values)
    {
    }

    const std::vector<std::string>& GetNames() { return m_names; }
    const std::vector<int>& GetValues() { return m_values; }
};

class ComponentProperty
{
public:
    ComponentProperty(const std::string& propertyName, const PropertyType type, const std::string& enumName = ""

    ) : m_propertyName(propertyName), m_type(type)
    {
        if (type == PropertyType::ENUM)
        {
            m_enumName = enumName;
        }
    }

    PropertyType GetPropertyType() const
    {
        return m_type;
    };

    const std::string& GetName()
    {
        return m_propertyName;
    }

    const std::string& GetEnumName()
    {
        return m_enumName;
    }

private:
    PropertyType m_type = PropertyType::NONE;
    // enumName if it is an enum;
    std::string m_enumName = "None";
    std::function<void(const std::any& value)> m_setter;
    std::function<std::any()> m_getter;
    std::string m_propertyName;
};


#endif //GRAFICOS_COMPONENTPROPERTY_H
