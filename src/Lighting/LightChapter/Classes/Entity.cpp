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

void Entity::SetScene(Scene* scene)
{
    m_scene = scene;
}


void Entity::UpdateTransform()
{
    CalculateLocalMatrix();
    if (!m_parent)
    {
        m_worldMat = m_localMat;
    }
    else
    {
        m_worldMat = m_parent->m_worldMat * m_localMat;
        m_normMat = glm::transpose(glm::inverse(glm::mat3(m_worldMat)));
    }
    for (auto& child : m_child)
    {
        child->UpdateTransform();
    }
}

void Entity::CalculateLocalMatrix()
{
    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, m_localPos);
    trans = trans * GetRotation();
    trans = glm::scale(trans, m_localScale);
    m_localMat = trans;
}

glm::mat4 Entity::GetRotation() const
{
    auto rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, glm::radians(m_localRot.z), Axis::Z);
    rot = glm::rotate(rot, glm::radians(m_localRot.x), Axis::X);
    rot = glm::rotate(rot, glm::radians(m_localRot.y), Axis::Y);
    return rot;
}
