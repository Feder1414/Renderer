//
// Created by USUARIO on 12/23/2025.
//

#include "Camera.h"

#include <iostream>

void Camera::ProcessMovement(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        m_position += m_forward * m_movementSPeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        m_position -= m_forward * m_movementSPeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        m_position -= glm::normalize(glm::cross(m_forward, m_up)) * m_movementSPeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        m_position += glm::normalize(glm::cross(m_forward, m_up)) * m_movementSPeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        const auto right = glm::normalize(glm::cross(m_forward, m_up));
        const auto up = glm::normalize(glm::cross(right, m_forward));
        m_position += up * m_movementSPeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        const auto right = glm::normalize(glm::cross(m_forward, m_up));
        const auto up = glm::normalize(glm::cross(right, m_forward));
        m_position -= up * m_movementSPeed * deltaTime;
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    return glm::lookAt(m_position, m_position + m_forward, m_up);

}

void Camera::UpdateCamRotation(GLFWwindow* window, double posx, double posy)

{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));

    const auto offsets = cam->m_mouseHandler->GetOffsets(window, posx, posy);
    cam->yaw += offsets.xOffset;
    cam->pitch += offsets.yOffset;

    if (cam->pitch > 89.f)
    {
        cam->pitch = 89.0f;
    }
    else if (cam->pitch < -89)
    {
        cam->pitch = 89;
    }
    cam->

    cam->m_forward.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    cam->m_forward.y = sin(glm::radians(cam->pitch));
    cam->m_forward.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
}

void Camera::UpdateFOV(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
    const auto fov = cam->m_fov - yoffset;

    std::cout << fov << std::endl;


    if (fov > cam->m_fovUpperLimit)
    {
        cam->m_fov = cam->m_fovUpperLimit;
    }
    else if (fov < cam->m_fovLowerLimit)
    {
        cam->m_fov = cam->m_fovLowerLimit;
    }
    else
    {
        cam->m_fov = fov;
    }
}
