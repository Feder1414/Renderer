//
// Created by USUARIO on 2/6/2026.
//

#ifndef GRAFICOS_GLWFWINDOW_H
#define GRAFICOS_GLWFWINDOW_H

#include <memory>

#include "Event.h"
#include "glfw3.h"

struct GLFWwindow;
class Engine;

class Window
{
public:
    void Initialize(Engine* engine);
    unsigned int GetWidth() const { return m_width; }
    unsigned int GetHeight() const { return m_height; }

    Event<int, int> m_OnViewPortSizeChange;
    Event<double, double> m_OnMouseMovement;
    Event<double, double> m_OnMouseScroll;

    void PollEvents() const;
    void SwapBuffers() const;
    void ProcessInput();

    double GetTime() const;
    bool WindowShouldClose() const;
    GLFWwindow* GetGLFWWindow() { return m_glfwWindow; }
    bool GetButtonPressed(int key) const;
    GLFWwindow* GetCurrentContext() const;
    void DisableCursor(bool disable);

private:
    unsigned int m_width = 1280;
    unsigned int m_height = 720;
    double m_lastFrameTime;
    GLFWwindow* m_glfwWindow;
    Engine* m_engine;
    bool m_cursorDisabled = true;


    bool m_tabPrevPressed = false;
    bool m_homePrevPressed = false;
    bool m_mouseCurrPressed = false;


    static void OnViewportSizeChanged(GLFWwindow* window, int width, int height);
    static void OnErrorWindow(int error, const char* description);
    static void OnMouseMove(GLFWwindow* glfwWindow, double xpos, double ypos);
    static void OnMouseScroll(GLFWwindow* glfwWindow, double xpos, double ypos);


};


#endif //GRAFICOS_GLWFWINDOW_H
