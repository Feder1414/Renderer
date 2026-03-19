//
// Created by USUARIO on 3/4/2026.
//

#ifndef GRAFICOS_FRUSTRUM_H
#define GRAFICOS_FRUSTRUM_H
#include "vec3.hpp"

struct Plane
{
    glm::vec3 m_normal = glm::vec3(0.1f);
    float m_distance = 0.0f;

    float GetSignedDistance(const glm::vec3& p) const
    {
        return glm::dot(m_normal, p) - m_distance;
    }

    Plane()
    {
    }

    Plane(glm::vec3 p, glm::vec3 normal)
    {
        m_normal = glm::normalize(normal);
        m_distance = glm::dot(p, m_normal);
    }
};

struct Frustum
{
public:
    Plane m_top;
    Plane m_down;
    Plane m_right;
    Plane m_left;
    Plane m_near;
    Plane m_far;
};


#endif //GRAFICOS_FRUSTRUM_H
