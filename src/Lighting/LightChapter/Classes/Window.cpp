//
// Created by USUARIO on 1/24/2026.
//


#include "glad/glad.h"
#include <format>
#include <iostream>


#include "glfw3.h"
#include "Window.h"

#include "Engine.h"


void Window::Initialize(Engine* engine)
{
    glfwInit();
    glfwSetErrorCallback(OnErrorWindow);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SRGB_CAPABLE, GLFW_TRUE);
    m_glfwWindow = glfwCreateWindow(m_width, m_height, "Window", NULL, NULL);
    if (m_glfwWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        throw std::exception("Failed to create the GLFW window. The application will terminate");
    }
    glfwMakeContextCurrent(m_glfwWindow);
    glfwSwapInterval(0);


    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    glfwSetWindowUserPointer(m_glfwWindow, this);

    //Set callbacks
    glfwSetFramebufferSizeCallback(m_glfwWindow, OnViewportSizeChanged);
    glfwSetCursorPosCallback(m_glfwWindow, OnMouseMove);
    glfwSetScrollCallback(m_glfwWindow, OnMouseScroll);


    m_engine = engine;
}


void Window::OnErrorWindow(int error, const char* description)
{
    std::cout << std::format("Error creating glfw window {}", description) << std::endl;
}

void Window::ProcessInput()
{
    if (glfwGetKey(m_glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(m_glfwWindow, GLFW_TRUE);
    }

    auto tabCurrPressed = glfwGetKey(m_glfwWindow, GLFW_KEY_TAB) == GLFW_PRESS;
    if (tabCurrPressed && !m_tabPrevPressed)
    {
        m_cursorDisabled = !m_cursorDisabled;
        auto cursorMode = m_cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

        glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, cursorMode);
    }
    m_tabPrevPressed = tabCurrPressed;

    auto homeCurrPresed = glfwGetKey(m_glfwWindow, GLFW_KEY_HOME) == GLFW_PRESS;
    if (homeCurrPresed && !m_homePrevPressed)
    {
        m_engine->ToggleProcessInput();
    }
    m_homePrevPressed = homeCurrPresed;
}

void Window::OnViewportSizeChanged(GLFWwindow* glfwWindow, int width, int height)
{
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->m_width = width;
    window->m_height = height;
    window->m_OnViewPortSizeChange.Notify(width, height);
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
    return glfwGetTime();
}

bool Window::WindowShouldClose() const
{
    return glfwWindowShouldClose(m_glfwWindow);
}

void Window::OnMouseMove(GLFWwindow* glfwWindow, double xpos, double ypos)
{
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->m_OnMouseMovement.Notify(xpos, ypos);
}

void Window::OnMouseScroll(GLFWwindow* glfwWindow, double xpos, double ypos)
{
    auto window = static_cast<Window*>(glfwGetWindowUserPointer(glfwWindow));
    window->m_OnMouseScroll.Notify(xpos, ypos);
}

void Window::DisableCursor(bool disable)
{
    auto cursorMode = disable ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

    glfwSetInputMode(m_glfwWindow, GLFW_CURSOR, cursorMode);
}

bool Window::GetButtonPressed(int key) const
{
    return glfwGetMouseButton(m_glfwWindow, key) == GLFW_PRESS;
}

GLFWwindow* Window::GetCurrentContext() const
{
    return glfwGetCurrentContext();
}
