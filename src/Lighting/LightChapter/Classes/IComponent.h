//
// Created by USUARIO on 1/24/2026.
//

#ifndef GRAFICOS_COMPONENT_H
#define GRAFICOS_COMPONENT_H

#include "Entity.h"

class IComponent
{
protected:
    Entity* entity;

public:
    virtual ~IComponent() = default;
    virtual void Update() = 0;
    void SetEntity(Entity* entityP) { entity = entityP; }
    Entity* GetEntity() const { return entity; }
};


#endif //GRAFICOS_COMPONENT_H
