//
// Created by USUARIO on 1/29/2026.
//

#ifndef GRAFICOS_ENGINE_H
#define GRAFICOS_ENGINE_H
#include <memory>
#include <thread>


#include "Scene.h"
#include "Renderer.h"
#include "Window.h"
#include "MouseHandler.h"
#include "imgui.h"
#include "GUI/Editor.h"


class Engine
{
public:
    static Engine* engine;
    void Initialize();
    void SetScene();
    void GameLoop();
    static double GetTime();
    static MouseHandler* GetMouseHandler();


    Scene* GetCurrScene() { return m_scene.get(); }

    void ToggleProcessInput() { m_processInput = !m_processInput; }

    Renderer* GetRenderer() const { return m_renderer.get(); }

private:
    void InitializeDefaultShaders();
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    std::unique_ptr<MouseHandler> m_mouseHandler;
    std::unique_ptr<Editor> m_editor;
    std::thread m_uiThread;
    int m_idOnChangeWindowResolution;
    int m_idOnMouseMovement;
    static double m_deltaTime;
    static double m_currFrameTime;
    static double m_lastFrameTime;
    bool m_processInput = true;


    void OnChangeWindowResolution(int m_widht, int m_height) const;
    void OnMouseMovement(double xpos, double ypos);
    void OnMouseScroll(double xpos, double ypos) const;
    bool ImGUiWantsCaptureMouse() const;
    bool ImGUiWantsCaptureKeyboard() const;
};


#endif //GRAFICOS_ENGINE_H
