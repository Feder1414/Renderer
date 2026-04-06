//
// Created by USUARIO on 3/4/2026.
//

#ifndef GRAFICOS_FRUSTRUM_H
#define GRAFICOS_FRUSTRUM_H
#include <array>

#include "vec3.hpp"
#include "glm.hpp"


struct Plane
{
    glm::vec3 m_normal = glm::vec3(0.1f);
    float m_distance = 0.0f;

    float GetSignedDistance(const glm::vec3& p) const
    {
        return glm::dot(m_normal, p) + m_distance;
    }

    Plane()
    {
    }


    Plane(glm::vec3 p, glm::vec3 normal)
    {
        m_normal = glm::normalize(normal);
        m_distance = glm::dot(p, m_normal);
    }

    void Normalize()
    {
        const float mag = glm::sqrt(m_normal.x * m_normal.x + m_normal.y * m_normal.y + m_normal.z * m_normal.z);
        m_normal /= mag;
        m_distance /= mag;
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

    Frustum()
    {
    };

    Frustum(const glm::mat4& viewMat, const glm::mat4& projMat)
    {
        auto viewProjMat = projMat * viewMat;

        // //Left plane
        // m_left.m_normal.x = viewProjMat[3][0] + viewProjMat[0][0];
        // m_left.m_normal.y = viewProjMat[3][1] + viewProjMat[0][1];
        // m_left.m_normal.z = viewProjMat[3][2] + viewProjMat[0][2];
        // m_left.m_distance = viewProjMat[3][3] + viewProjMat[0][3];
        // m_left.Normalize();
        //
        // //Right plane
        // m_right.m_normal.x = viewProjMat[3][0] - viewProjMat[0][0];
        // m_right.m_normal.y = viewProjMat[3][1] - viewProjMat[0][1];
        // m_right.m_normal.z = viewProjMat[3][2] - viewProjMat[0][2];
        // m_right.m_distance = viewProjMat[3][3] - viewProjMat[0][3];
        // m_right.Normalize();
        //
        //
        // //Top plane
        // m_top.m_normal.x = viewProjMat[3][0] - viewProjMat[1][0];
        // m_top.m_normal.y = viewProjMat[3][1] - viewProjMat[1][1];
        // m_top.m_normal.z = viewProjMat[3][2] - viewProjMat[1][2];
        // m_top.m_distance = viewProjMat[3][3] - viewProjMat[1][3];
        // m_top.Normalize();
        //
        // //Bottom plane
        // m_down.m_normal.x = viewProjMat[3][0] + viewProjMat[1][0];
        // m_down.m_normal.y = viewProjMat[3][1] + viewProjMat[1][1];
        // m_down.m_normal.z = viewProjMat[3][2] + viewProjMat[1][2];
        // m_down.m_distance = viewProjMat[3][3] + viewProjMat[1][3];
        // m_down.Normalize();
        //
        // // Near plane
        // m_near.m_normal.x = viewProjMat[3][0] + viewProjMat[2][0];
        // m_near.m_normal.y = viewProjMat[3][1] + viewProjMat[2][1];
        // m_near.m_normal.z = viewProjMat[3][2] + viewProjMat[2][2];
        // m_near.m_distance = viewProjMat[3][3] + viewProjMat[2][3];
        // m_near.Normalize();
        //
        // // Far plane
        // m_far.m_normal.x = viewProjMat[3][0] - viewProjMat[2][0];
        // m_far.m_normal.y = viewProjMat[3][1] - viewProjMat[2][1];
        // m_far.m_normal.z = viewProjMat[3][2] - viewProjMat[2][2];
        // m_far.m_distance = viewProjMat[3][3] - viewProjMat[2][3];
        // m_far.Normalize();

        //Left plane
        m_left.m_normal.x = viewProjMat[0][3] + viewProjMat[0][0];
        m_left.m_normal.y = viewProjMat[1][3] + viewProjMat[1][0];
        m_left.m_normal.z = viewProjMat[2][3] + viewProjMat[2][0];
        m_left.m_distance = viewProjMat[3][3] + viewProjMat[3][0];
        m_left.Normalize();

        //Right plane
        m_right.m_normal.x = viewProjMat[0][3] - viewProjMat[0][0];
        m_right.m_normal.y = viewProjMat[1][3] - viewProjMat[1][0];
        m_right.m_normal.z = viewProjMat[2][3] - viewProjMat[2][0];
        m_right.m_distance = viewProjMat[3][3] - viewProjMat[3][0];
        m_right.Normalize();


        //Top plane
        m_top.m_normal.x = viewProjMat[0][3] - viewProjMat[0][1];
        m_top.m_normal.y = viewProjMat[1][3] - viewProjMat[1][1];
        m_top.m_normal.z = viewProjMat[2][3] - viewProjMat[2][1];
        m_top.m_distance = viewProjMat[3][3] - viewProjMat[3][1];
        m_top.Normalize();

        //Bottom plane
        m_down.m_normal.x = viewProjMat[0][3] + viewProjMat[0][1];
        m_down.m_normal.y = viewProjMat[1][3] + viewProjMat[1][1];
        m_down.m_normal.z = viewProjMat[2][3] + viewProjMat[2][1];
        m_down.m_distance = viewProjMat[3][3] + viewProjMat[3][1];
        m_down.Normalize();

        // Near plane
        m_near.m_normal.x = viewProjMat[0][3] + viewProjMat[0][2];
        m_near.m_normal.y = viewProjMat[1][3] + viewProjMat[1][2];
        m_near.m_normal.z = viewProjMat[2][3] + viewProjMat[2][2];
        m_near.m_distance = viewProjMat[3][3] + viewProjMat[3][2];
        m_near.Normalize();

        // Far plane
        m_far.m_normal.x = viewProjMat[0][3] - viewProjMat[0][2];
        m_far.m_normal.y = viewProjMat[1][3] - viewProjMat[1][2];
        m_far.m_normal.z = viewProjMat[2][3] - viewProjMat[2][2];
        m_far.m_distance = viewProjMat[3][3] - viewProjMat[3][2];
        m_far.Normalize();
    }
};


#endif //GRAFICOS_FRUSTRUM_H
