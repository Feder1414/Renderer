//
// Created by USUARIO on 4/2/2026.
//

#include "ShadowMapViewer.h"

#include "Renderer.h"

#include "imgui.h"

ShadowMapViewer::ShadowMapViewer(Renderer* renderer) : IWidget("Shadow map viewer")
{
    m_renderer = renderer;
    m_shadowViewPort = renderer->GetShadowPassDebugViewport();
}

void ShadowMapViewer::Render()
{
    if (!ImGui::Begin(m_windowName.c_str()))
    {
        ImGui::End();
    }

    auto currSize = ImGui::GetContentRegionAvail();

    if (currSize.x != m_prevWidth || currSize.y != m_prevHeight)
    {
        m_shadowViewPort->SetSizeFrameBuffer(currSize.x, currSize.y);
    }

    m_prevWidth = currSize.x;
    m_prevHeight = currSize.y;


    ImGui::Image((ImTextureID)(uintptr_t)m_shadowViewPort->GetColorTextureId(),
                 currSize,
                 ImVec2(0, 1),
                 ImVec2(1, 0));


    ImGui::End();
}
