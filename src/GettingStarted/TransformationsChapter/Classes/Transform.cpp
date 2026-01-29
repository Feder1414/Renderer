//
// Created by USUARIO on 12/19/2025.
//

#include "Transform.h"

glm::mat4 Transform::GetAllTransformMatrix() const
{
    auto trans = glm::mat4(1.0f);
    trans = glm::scale(trans, m_scale);
    trans = GetRotation() * trans;
    trans = glm::translate(trans, m_position);
    return trans;
}


glm::mat4 Transform::GetRotation() const
{
    auto rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, glm::radians(m_rotation.z), Axis::Z);
    rot = glm::rotate(rot, glm::radians(m_rotation.x), Axis::X);
    rot = glm::rotate(rot, glm::radians(m_rotation.y), Axis::Y);
    return rot;
}

void Transform::SetRotation(glm::vec3 rotation)
{
    m_rotation = rotation;
}

void Transform::SetScale(glm::vec3 scale)
{
    m_scale = scale;
}

void Transform::SetTranslation(glm::vec3 position)
{
    m_position = position;
}
