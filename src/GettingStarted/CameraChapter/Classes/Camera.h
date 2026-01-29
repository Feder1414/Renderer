//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_CAMERA_H
#define GRAFICOS_CAMERA_H
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "MouseHandler.h"


class Camera
{
private:
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, -1.0f);
    float yaw = 0.0f;
    float pitch = 0.0f;
    float m_fov = 45.0f;;
    float m_fovLowerLimit = 1.0f;
    float m_fovUpperLimit = 45.0f;

    MouseHandler* m_mouseHandler;

    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    float m_movementSPeed = 0.5f;

public:
    void ProcessMovement(GLFWwindow* window, float deltaTime);

    void SetMouseHandler(MouseHandler* mouseHandler)
    {
        m_mouseHandler = mouseHandler;
    }

    static void CalculateForwardVector(GLFWwindow* window, double posx, double posy);

    glm::mat4 GetViewMatrix() const;

    float GetFov() const
    {
        return m_fov;
    }


    static void UpdateFOV(GLFWwindow* window, double xoffset, double yoffset);
};


#endif //GRAFICOS_CAMERA_H
