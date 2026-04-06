//
// Created by USUARIO on 3/27/2026.
//

#ifndef GRAFICOS_SCENEVIEW_H
#define GRAFICOS_SCENEVIEW_H
#include "IWidget.h"


class Engine;
class Renderer;

class SceneView : public IWidget
{
public:
    SceneView(Renderer* renderer, Engine* engine) : IWidget("Scene view"), m_renderer(renderer), m_engine(engine)
    {
    }


    bool m_firstFrame = false;
    unsigned int m_width = 1;
    unsigned int m_height = 1;


    void Render() override;

private:
    Renderer* m_renderer;
    Engine* m_engine;
};


#endif //GRAFICOS_SCENEVIEW_H
