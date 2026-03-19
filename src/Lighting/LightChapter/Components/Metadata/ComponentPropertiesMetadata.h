//
// Created by USUARIO on 2/17/2026.
//

#ifndef GRAFICOS_COMPONENTWITHPROPERTIES_H
#define GRAFICOS_COMPONENTWITHPROPERTIES_H
#include "ComponentProperty.h"

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
};


#endif //GRAFICOS_COMPONENTWITHPROPERTIES_H
