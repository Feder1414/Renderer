//
// Created by USUARIO on 3/4/2026.
//

#ifndef GRAFICOS_IBOUNDINGVOLUME_H
#define GRAFICOS_IBOUNDINGVOLUME_H


struct Plane;
class Entity;
class Frustum;

class IBoundingVolume
{
public:
    virtual ~IBoundingVolume() = default;
    virtual bool IsOnFrustum(const Frustum& frustum) const = 0;
    virtual bool IsOnForwardPlane(const Plane& plane) const = 0;
    virtual std::unique_ptr<IBoundingVolume> CalculateWorldBB(Entity* entity) = 0;
};


#endif //GRAFICOS_IBOUNDINGVOLUME_H
