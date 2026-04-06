//
// Created by USUARIO on 2/17/2026.
//

#ifndef GRAFICOS_COMPONENTWITHPROPERTIES_H
#define GRAFICOS_COMPONENTWITHPROPERTIES_H
#include "ComponentProperty.h"


inline size_t GetUniqueForComponentId()
{
    static size_t currId = 0;
    return currId++;
}

template <class derived>
class ComponentPropertiesMetadata
{
public:
    static auto& GetPropertiesMetadata()
    {
        static std::unordered_map<std::string, ComponentProperty> propertiesMetadata;
        return propertiesMetadata;
    }

    static auto& GetAlreadyInitialized()
    {
        static bool alreadyInitialized = false;
        return alreadyInitialized;
    }

    static auto& GetMetadataComponentName()
    {
        static std::string componentName = "NoComponentNameSet";
        return componentName;
    }

    static size_t GetComponentMetadataId()
    {
        static size_t GetUniqueId = GetUniqueForComponentId();
        return GetUniqueId;
    }
};


#endif //GRAFICOS_COMPONENTWITHPROPERTIES_H
