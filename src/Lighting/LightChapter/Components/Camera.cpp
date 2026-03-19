//
// Created by USUARIO on 12/23/2025.
//


#include <iostream>

#include "Entity.h"

#include "Camera.h"

#include "Engine.h"


void Camera::ProcessMovement(GLFWwindow* window, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        //m_transform.m_position += m_forward * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos += GetForwardCam() * m_movementSPeed * deltaTime;
        entity->SetPosGlobal(camPos);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // m_transform.m_position -= m_forward * m_movementSPeed * deltaTime;
        auto camPos = entity->GetWorldPos();
        camPos -= GetForwardCam() * m_movementSPeed * deltaTime;
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


    //worldMat[2] = -worldMat[2];
    //auto inverse = glm::inverse(worldMat);
    //std::cout << inverse[0].x << std::endl;
    auto lookat = glm::lookAt(entity->GetWorldPos(), entity->GetWorldPos() - glm::vec3(worldMat[2]), entity->GetUp());

    return lookat;
}

glm::mat4 Camera::GetProjMatrix(const float aspectRatio) const
{
    return glm::perspective(glm::radians(m_fov), aspectRatio, m_nearPlane, m_farPlane);
}


void Camera::UpdateCamRotation(float offsetX, float offsetY)

{
    yaw -= offsetX;
    pitch -= offsetY;

    if (pitch > 89.f)
    {
        pitch = 89.0f;
    }
    else if (pitch < -89)
    {
        pitch = -89;
    }

    auto localRot = GetEntity()->GetLocalRot();
    localRot.x = -pitch;
    localRot.y = yaw;
    GetEntity()->SetLocalRot(localRot);
    // cam->m_forward.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    // cam->m_forward.y = sin(glm::radians(cam->pitch));
    // cam->m_forward.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
}

void Camera::UpdateFOV(double xoffset, double yoffset)
{
    const auto fov = m_fov - yoffset;

    std::cout << fov << std::endl;


    SetFov(fov);
}

void Camera::SetFov(double fov)
{
    if (fov > m_fovUpperLimit)
    {
        m_fov = m_fovUpperLimit;
    }
    else if (fov < m_fovLowerLimit)
    {
        m_fov = m_fovLowerLimit;
    }
    else
    {
        m_fov = fov;
    }
}

Camera::Camera()
{
    m_mouseHandler = Engine::GetMouseHandler();
    m_mouseHandler->m_onMouseMovement.AddListener([this](auto&& PH1, auto&& PH2)
    {
        UpdateCamRotation(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
    m_mouseHandler->m_onScrollMouse.AddListener([this](auto&& PH1, auto&& PH2)
    {
        UpdateFOV(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
}

glm::vec3 Camera::GetForwardCam() const
{
    return -GetEntity()->GetWorldMat()[2];
}

void Camera::SetComponentMetadata()
{
}
