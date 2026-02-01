//
// Created by USUARIO on 12/19/2025.
//

#include "Transform.h"

void Transform::GetAllTransformMatrix()
{
    auto trans = glm::mat4(1.0f);
    trans = glm::translate(trans, m_position);
    trans = trans * GetRotation();
    trans = glm::scale(trans, m_scale);
    m_matrix = trans;
}


glm::mat4 Transform::GetRotation()
{
    auto rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, glm::radians(m_rotation.z), Axis::Z);
    rot = glm::rotate(rot, glm::radians(m_rotation.x), Axis::X);
    rot = glm::rotate(rot, glm::radians(m_rotation.y), Axis::Y);
    return rot;
}


glm::mat3 Transform::GetNormalMatrix()
{
    auto normalMatrix = glm::transpose(glm::inverse(glm::mat3(m_matrix)));

    return normalMatrix;
}
