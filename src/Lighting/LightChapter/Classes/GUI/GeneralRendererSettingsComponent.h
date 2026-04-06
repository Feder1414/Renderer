//
// Created by USUARIO on 3/6/2026.
//

#ifndef GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
#define GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
#include <memory>

#include "IWidget.h"
#include "ShadowMapViewer.h"


class Renderer;

class GeneralRendererSettingsComponent : public IWidget
{
    Renderer* m_renderer = nullptr;
    std::unique_ptr<ShadowMapViewer> m_shadowMapViewer = nullptr;
    //
    // bool m_renderBB = false;
    // bool m_fogEffect = false;
    // bool m_renderS

    bool m_seeShadowMap = false;

public:
    GeneralRendererSettingsComponent(Renderer* renderer) : IWidget("General render settings")
    {
        m_renderer = renderer;
        m_shadowMapViewer = std::make_unique<ShadowMapViewer>(renderer);
    }

    void Render() override;
};


#endif //GRAFICOS_GENERALRENDERERSETTINGSCOMPONENT_H
