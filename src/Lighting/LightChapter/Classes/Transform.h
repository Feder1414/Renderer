//
// Created by USUARIO on 12/19/2025.
//

#ifndef GRAFICOS_TRANSFORM_H
#define GRAFICOS_TRANSFORM_H

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

class Axis
{
public:
    static inline  glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
    static inline  glm::vec3 Y = glm::vec3(0.0f, 1.0f, 0.0f);
    static inline  glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);
};

enum class AxisIndex
{
    X = 0,
    Y = 1,
    Z = 2,
};


class Transform
{
private:


public:
    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_rotation = glm::vec3(0, 0, 0);
    glm::vec3 m_scale = glm::vec3(1, 1, 1);
    glm::mat4 GetAllTransformMatrix() const;
    glm::mat4 GetRotation() const;
    glm::mat3 GetNormalMatrix() const;
    void SetTranslation(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);

};


#endif //GRAFICOS_TRANSFORM_H
