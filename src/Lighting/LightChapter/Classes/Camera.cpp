//
// Created by USUARIO on 12/23/2025.
//


#include <iostream>

#include "Entity.h"

#include "Camera.h"


void Camera::ProcessMovement(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        //m_transform.m_position += m_forward * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos += entity->GetForward() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // m_transform.m_position -= m_forward * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos -= entity->GetForward() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        //m_transform.m_position -= glm::normalize(glm::cross(m_forward, m_up)) * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos -= entity->GetRight() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        //m_transform.m_position += glm::normalize(glm::cross(m_forward, m_up)) * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos += entity->GetRight() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        // const auto right = glm::normalize(glm::cross(m_forward, m_up));
        // const auto up = glm::normalize(glm::cross(right, m_forward));
        // m_transform.m_position += up * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos += entity->GetUp() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        // const auto right = glm::normalize(glm::cross(m_forward, m_up));
        // const auto up = glm::normalize(glm::cross(right, m_forward));
        // m_transform.m_position -= up * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos -= entity->GetUp() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
}

glm::mat4 Camera::GetViewMatrix() const
{
    //return glm::lookAt(m_transform.m_position, m_transform.m_position + m_forward, m_up);
    //return glm::lookAt(GetEntity()->GetWorldPos(), GetEntity()->GetWorldPos() - GetEntity()->GetForward(), m_up);
    auto worldMat = entity->GetWorldMat();
    worldMat[2] = -worldMat[2];
    return glm::inverse(worldMat);
}

glm::mat4 Camera::GetProjMatrix(const float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}


void Camera::CalculateForwardVector(GLFWwindow* window, double posx, double posy)

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
        cam->pitch = -89;
    }

    auto localRot = cam->GetEntity()->GetLocalRot();
    localRot.x = -cam->pitch;
    localRot.y = cam->yaw;
    cam->GetEntity()->SetLocalRot(localRot);
    // cam->m_forward.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    // cam->m_forward.y = sin(glm::radians(cam->pitch));
    // cam->m_forward.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
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
