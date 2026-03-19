//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_MOUSEHANDLER_H
#define GRAFICOS_MOUSEHANDLER_H
#include "Event.h"


struct XYOffset
{
    float xOffset;
    float yOffset;
};

class MouseHandler
{
private:
    float m_lastX, m_lastY;
    float m_sensitivity = 0.1f;
    bool m_firstMouse = true;

public:
    MouseHandler(const float width, const float height)
    {
        m_lastX = width / 2.0f;
        m_lastY = height / 2.0f;
    }
    Event<float, float> m_onMouseMovement;
    Event<float, float> m_onScrollMouse;

    XYOffset GetOffsets(double xpos, double ypos);

    void SetMouseSensitivity(const float mouseSensitivity) { m_sensitivity = mouseSensitivity; }

    void NotifyMouseMovement(double xpos, double ypos);
    void NotifyMouseScroll(double xpos, double ypos);
};


#endif //GRAFICOS_MOUSEHANDLER_H
