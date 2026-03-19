//
// Created by USUARIO on 2/16/2026.
//

#ifndef GRAFICOS_GENERICCOMPONENT_H
#define GRAFICOS_GENERICCOMPONENT_H
#include <any>
#include <string>
#include <unordered_map>

#include "IWidget.h"


class ComponentProperty;

class GenericComponent : IWidget
{
    std::unordered_map<std::string, std::any> m_genericComponentValues = {};

    GenericComponent(ComponentProperty& property);
};


#endif //GRAFICOS_GENERICCOMPONENT_H
