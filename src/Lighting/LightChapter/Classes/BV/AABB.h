//
// Created by USUARIO on 3/4/2026.
//

#ifndef GRAFICOS_AABB_H
#define GRAFICOS_AABB_H
#include <memory>

#include "IBoundingVolume.h"
#include "vec3.hpp"


class AABB : public IBoundingVolume
{
public:
    bool IsOnFrustum(const Frustum& frustum) const override;
    bool IsOnForwardPlane(const Plane& plane) const override;
    std::unique_ptr<IBoundingVolume> CalculateWorldBB(Entity* entity) override;
    AABB CalculateWorldAABB(Entity* entity) const;
    AABB(const glm::vec3& min, const glm::vec3& max);
    AABB(const glm::vec3& center, float ix, float iy, float iz);
    glm::vec3 GetCenter() const { return m_center; }
    glm::vec3 GetExtents() const { return m_extents; }

private:
    void TransformToWorld(::Entity* entity, glm::vec3& newCenter, float& newIx, float& newIy, float& newIz) const;
    glm::vec3 m_center = glm::vec3(0.0F);
    glm::vec3 m_extents = glm::vec3(0.0f);
};


#endif //GRAFICOS_AABB_H
