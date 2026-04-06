//
// Created by USUARIO on 4/2/2026.
//

#ifndef GRAFICOS_SHADOWMAPVIEWER_H
#define GRAFICOS_SHADOWMAPVIEWER_H
#include "IWidget.h"


class ViewPort;
class Renderer;

class ShadowMapViewer : IWidget
{
public:
    ShadowMapViewer(Renderer* renderer);
    void Render() override;

private:
    Renderer* m_renderer;
    ViewPort* m_shadowViewPort;

    unsigned int m_prevWidth;
    unsigned int m_prevHeight;
};


#endif //GRAFICOS_SHADOWMAPVIEWER_H
