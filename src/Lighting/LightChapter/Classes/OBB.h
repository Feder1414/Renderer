//
// Created by USUARIO on 4/4/2026.
//

#ifndef GRAFICOS_OBB_H
#define GRAFICOS_OBB_H
#include <array>

#include "glm.hpp"

namespace
{
    std::array<glm::vec3, 8> frustumNDCCoordinates = {
        //Front
        glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 1.0f, 1.0f),
        glm::vec3(-1.0f, -1.0f, -1.0f),
        //Back
        glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, -1.0f, -1.0f),
        glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, -1.0f, -1.0f)
    };
}

class OBB
{
    glm::vec3 m_center = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat3 m_orientation = glm::mat3(0.1f);
    glm::vec3 m_extents = glm::vec3(0.0f);


    OBB(const glm::mat4& viewProj, const glm::mat3& vectorBase)
    {
        m_orientation = vectorBase;
        std::array<glm::vec3, 8> frustumWorldCoordinates = {};

        auto invViewProj = glm::inverse(viewProj);
        for (int i = 0; i < frustumNDCCoordinates.size(); i++)
        {
            auto worldCoordinatesFrustumPoint = invViewProj * glm::vec4(frustumNDCCoordinates[i], 1.0f);
            frustumWorldCoordinates[i] = glm::vec3(worldCoordinatesFrustumPoint) / worldCoordinatesFrustumPoint.w;
        }

        for (int j = 0; j < 3; j++)
        {
            float minProjPoint = std::numeric_limits<float>::min();
            float maxProjPoint = std::numeric_limits<float>::max();
            for (int i = 0; i < frustumWorldCoordinates.size(); i++)
            {
                auto projPoint = glm::dot(frustumWorldCoordinates[i], vectorBase[j]);
                minProjPoint = std::min(minProjPoint, projPoint);
                maxProjPoint = std::max(maxProjPoint, projPoint);
            }
            auto differenceMaxMinProjPoint = glm::length(maxProjPoint - minProjPoint);


            if (j == 0)
            {
                m_extents.x = differenceMaxMinProjPoint * 0.5f;;
            }
            else if (j == 1)
            {
                m_extents.y = differenceMaxMinProjPoint * 0.5f;
            }
            else if (j == 2)
            {
                m_extents.z = differenceMaxMinProjPoint * 0.5f;
            }
        }

        m_center = (-m_extents + m_extents) * 0.5f;
    }
};


#endif //GRAFICOS_OBB_H
