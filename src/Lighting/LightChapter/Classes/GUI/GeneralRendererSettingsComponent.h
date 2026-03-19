//
// Created by USUARIO on 3/6/2026.
//

#ifndef GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
#define GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
#include "IWidget.h"


class Renderer;

class GeneralRendererSettingsComponent : public IWidget
{
    Renderer* m_renderer = nullptr;
    //
    // bool m_renderBB = false;
    // bool m_fogEffect = false;
    // bool m_renderS

public:
    GeneralRendererSettingsComponent(Renderer* renderer) { m_renderer = renderer; }

    void Render() override;
};


#endif //GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
