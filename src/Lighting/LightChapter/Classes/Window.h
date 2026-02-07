//
// Created by USUARIO on 2/6/2026.
//

#ifndef GRAFICOS_GLWFWINDOW_H
#define GRAFICOS_GLWFWINDOW_H

#include <memory>

#include "Engine.h"
#include "Event.h"
#include "Window.h"
struct GLFWwindow;
class Window;

struct WindowContext
{
    Window* window;
    Engine* engine;
};

class Window
{
public:
    void Initialize(Engine* engine);
    unsigned int GetWidth() const { return m_width; }
    unsigned int GetHeight() const { return m_height; }

    Event<int, int> viewPortSizeChange;

    void PollEvents() const;
    void SwapBuffers() const;
    void ProcessInput();

    double GetTime() const;
    bool WindowShouldClose() const;
    GLFWwindow* GetGLFWWindow() { return m_glfwWindow; }

private:
    unsigned int m_width = 1280;
    unsigned int m_height = 720;
    double m_lastFrameTime;
    GLFWwindow* m_glfwWindow;
    Engine* m_engine;
    bool m_cursorDisabled = true;


    static void OnViewportSizeChanged(GLFWwindow* window, int width, int height);
    static void OnErrorWindow(int error, const char* description);
};


#endif //GRAFICOS_GLWFWINDOW_H
