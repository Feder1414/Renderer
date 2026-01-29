//
// Created by USUARIO on 1/23/2026.
//

#include "Entity.h"

#include "IComponent.h"
#include "ModelRenderInfo.h"

void Entity::SetModelRenderInfo(std::unique_ptr<ModelRenderInfo> modelRenderInfo)
{
    m_modelRenderInfo = std::move(modelRenderInfo);
    m_modelRenderInfo->SetEntity(this);
}

ModelRenderInfo* Entity::GetModelRenderInfo() const
{
    return m_modelRenderInfo.get();
}

void Entity::AddComponent(std::unique_ptr<IComponent> component)
{
    component->SetEntity(this);
    m_components.push_back(std::move(component));
}


