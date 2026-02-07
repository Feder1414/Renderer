//
// Created by USUARIO on 1/24/2026.
//

#include "Window.h"

#include <format>
#include <iostream>

#include "glad/glad.h"
#include "glfw3.h"


void Window::Initialize(Engine* engine)
{
    glfwInit();
    glfwSetErrorCallback(OnErrorWindow);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_glfwWindow = glfwCreateWindow(m_width, m_height, "Window", NULL, NULL);
    if (m_glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::exception("Failed to create the GLFW window. The application will terminate");
    }
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetWindowUserPointer(m_glfwWindow, this);
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSetFramebufferSizeCallback(m_glfwWindow, OnViewportSizeChanged);
    m_engine = engine;
}

void Window::OnErrorWindow(int error, const char* description)
{
    std::cout << std::format("Error creating glfw window {}", description);
}

void Window::ProcessInput()
{
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
    }
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        auto cursorMode = m_cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
        glfwSetInputMode(m_glfwWindow, GLFW_TRUE, cursorMode);
    }
}

void Window::OnViewportSizeChanged(GLFWwindow* glfwWindow, int width, int height)
{
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->m_width = width;
    window->m_height = height;
    window->viewPortSizeChange.Notify(width, height);
}

void Window::PollEvents() const
{
    glfwPollEvents();
}

void Window::SwapBuffers() const
{
    glfwSwapBuffers(m_glfwWindow);
}

double Window::GetTime() const
{
    glfwGetTime();
}

bool Window::WindowShouldClose() const
{
    return glfwWindowShouldClose(m_glfwWindow);
}
