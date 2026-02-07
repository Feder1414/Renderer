//
// Created by USUARIO on 1/29/2026.
//

#ifndef GRAFICOS_ENGINE_H
#define GRAFICOS_ENGINE_H
#include <memory>


class Renderer;
class Window;
class Scene;


class Engine
{
public:
    static Engine* engine;
    void Initialize();
    void SetScene();
    void GameLoop();
    static double GetTime();

private:
    std::unique_ptr<Window> m_window;
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Scene> m_scene;
    int m_idOnChangeWindowResolution;
    static double m_deltaTime;
    static double m_currFrameTime;
    static double m_lastFrameTime;


    void OnChangeWindowResolution(int m_widht, int m_height) const;
};


#endif //GRAFICOS_ENGINE_H
