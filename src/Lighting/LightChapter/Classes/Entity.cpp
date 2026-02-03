//
// Created by USUARIO on 1/23/2026.
//

#include "Entity.h"
#include <gtx/matrix_decompose.hpp>
#include <gtx/euler_angles.hpp>

#include "IComponent.h"
#include "ModelRenderInfo.h"


Entity::Entity()
{
    UpdateTransform();
}


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
    }
    m_normMat = glm::transpose(glm::inverse(glm::mat3(m_worldMat)));
    m_worldPos = glm::vec3(m_worldMat * glm::vec4(m_localPos, 1.0f));


    glm::quat rotation;
    glm::vec3 skew;
    glm::vec4 perspective;

    glm::decompose(m_worldMat, m_worldScale, rotation, m_worldPos, skew, perspective);
    glm::vec3 eulerRads = glm::eulerAngles(rotation);
    m_worldRot = glm::degrees(eulerRads);


    m_right = glm::normalize(glm::vec3(m_worldMat[0]));
    m_up = glm::normalize(glm::vec3(m_worldMat[1]));
    m_forward = glm::normalize(glm::vec3(m_worldMat[2]));

    for (auto& child : m_child)
    {
        child->UpdateTransform();
    }
}

void Entity::CalculateLocalMatrix()
{
    m_localMat = glm::translate(glm::mat4(1.0f), m_localPos) * GetRotation() * glm::scale(
        glm::mat4(1.0f), m_localScale);
}

glm::mat4 Entity::GetRotation() const
{
    // auto rot = glm::mat4(1.0f);
    // rot = glm::rotate(rot, glm::radians(m_localRot.z), Axis::Z);
    // rot = glm::rotate(rot, glm::radians(m_localRot.x), Axis::X);
    // rot = glm::rotate(rot, glm::radians(m_localRot.y), Axis::Y);

    return glm::yawPitchRoll(glm::radians(m_localRot.y), glm::radians(m_localRot.x), glm::radians(m_localRot.z));
}
