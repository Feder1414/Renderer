//
// Created by USUARIO on 3/4/2026.
//

#include "AABB.h"
#include "AABB.h"
#include "AABB.h"

#include "Entity.h"
#include "Frustum.h"

bool AABB::IsOnFrustum(const Frustum& frustum)
{
    if (!IsOnForwardPlane(frustum.m_down)) return false;
    if (!IsOnForwardPlane(frustum.m_top)) return false;
    if (!IsOnForwardPlane(frustum.m_right)) return false;
    if (!IsOnForwardPlane(frustum.m_left)) return false;
    if (!IsOnForwardPlane(frustum.m_near)) return false;
    if (!IsOnForwardPlane(frustum.m_far)) return false;
    return true;
    // auto isOnDownPlane = IsOnForwardPlane(frustum.m_down);
    // auto isOnTopPlane = IsOnForwardPlane(frustum.m_top);
    // auto isOnRightPlane = IsOnForwardPlane(frustum.m_right);
    // auto isOnLeftPlane = IsOnForwardPlane(frustum.m_left);
    // auto isOnNearPlane = IsOnForwardPlane(frustum.m_near);
    // auto isOnFarPlane = IsOnForwardPlane(frustum.m_far);
    //
    // //return true;
    // return isOnDownPlane && isOnTopPlane && isOnRightPlane && isOnLeftPlane && isOnNearPlane && isOnFarPlane;
    //
    // return isOnFarPlane && isOnNearPlane;
}

std::unique_ptr<IBoundingVolume> AABB::CalculateWorldBB(Entity* entity)
{
    float newIy;

    float newIx;
    float newIz;


    glm::vec3 newCenter;
    TransformToWorld(entity, newCenter, newIx, newIy, newIz);


    return std::move(std::make_unique<AABB>(glm::vec3(newCenter), newIx, newIy, newIz));
}

AABB AABB::CalculateWorldAABB(Entity* entity) const
{
    float newIy;

    float newIx;
    float newIz;


    glm::vec3 newCenter;
    TransformToWorld(entity, newCenter, newIx, newIy, newIz);

    return AABB(glm::vec3(newCenter), newIx, newIy, newIz);
}

void AABB::TransformToWorld(Entity* entity, glm::vec3& newCenter, float& newIx, float& newIy, float& newIz) const
{
    auto globalMat = entity->GetWorldMat();

    auto right = glm::vec3(globalMat[0]) * m_extents.x;
    auto up = glm::vec3(globalMat[1]) * m_extents.y;
    auto forward = glm::vec3(globalMat[2]) * m_extents.z;


    auto upGlobal = glm::vec3(0.0f, 1.0f, 0.0f);
    auto rightGlobal = glm::vec3(1.0f, 0.0f, 0.0f);
    auto forwardGlobal = glm::vec3(0.0f, 0.0f, 1.0f);

    newIy = std::abs(glm::dot(upGlobal, up)) +
        std::abs(glm::dot(upGlobal, right)) + std::abs(glm::dot(upGlobal, forward));
    newIx = std::abs(glm::dot(rightGlobal, up)) +
        std::abs(glm::dot(rightGlobal, right)) + std::abs(glm::dot(rightGlobal, forward));
    newIz = std::abs(glm::dot(forwardGlobal, up)) +
        std::abs(glm::dot(forwardGlobal, right)) + std::abs(glm::dot(forwardGlobal, forward));

    newCenter = glm::vec3(entity->GetWorldMat() * glm::vec4(m_center, 1.0f));
};


bool AABB::IsOnForwardPlane(const Plane& plane)
{
    const float r = m_extents.x * std::abs(plane.m_normal.x) +
        m_extents.y * std::abs(plane.m_normal.y) + m_extents.z * std::abs(plane.m_normal.z);


    return -r <= plane.GetSignedDistance(m_center);
}


AABB::AABB(const glm::vec3& min, const glm::vec3& max)
{
    m_center = (min + max) * 0.5f;
    m_extents = glm::vec3(max.x - m_center.x, max.y - m_center.y, max.z - m_center.z);
}

AABB::AABB(const glm::vec3& center, float ix, float iy, float iz)
{
    m_center = center;
    m_extents = glm::vec3(ix, iy, iz);
}
