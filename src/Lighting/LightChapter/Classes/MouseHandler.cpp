//
// Created by USUARIO on 12/23/2025.
//

#include "MouseHandler.h"

#include "glfw3.h"

XYOffset MouseHandler::GetOffsets(double xpos, double ypos)
{
    if (m_firstMouse)
    {
        m_firstMouse = false;
        return {.xOffset = 0, .yOffset = 0};
    }

    float xOffset = (xpos - m_lastX) * m_sensitivity;
    float yOffset = (m_lastY - ypos) * m_sensitivity;

    m_lastX = xpos;
    m_lastY = ypos;

    return {.xOffset = xOffset, .yOffset = yOffset};
}

void MouseHandler::NotifyMouseMovement(double xpos, double ypos)
{
    auto offsets = GetOffsets(xpos, ypos);
    auto xposF = static_cast<float>(offsets.xOffset);
    auto yposF = static_cast<float>(offsets.yOffset);

    m_onMouseMovement.Notify(xposF, yposF);
}

void MouseHandler::NotifyMouseScroll(double xpos, double ypos)
{
    auto xposF = static_cast<float>(xpos);
    auto yposF = static_cast<float>(ypos);
    m_onScrollMouse.Notify(xposF, yposF);
}

