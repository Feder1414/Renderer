//
// Created by USUARIO on 3/27/2026.
//

#include "SceneView.h"

#include "imgui.h"
#include "Renderer.h"
#include "Engine.h"

void SceneView::Render()
{
    if (!ImGui::Begin(m_windowName.c_str()))
    {
        ImGui::End();
    }

    auto currSize = ImGui::GetContentRegionAvail();

    if (currSize.x != m_width || currSize.y != m_height)
    {
        m_renderer->SetSceneViewPortResolution(currSize.x, currSize.y);
    }

    m_width = currSize.x;
    m_height = currSize.y;


    ImGui::Image((ImTextureID)(uintptr_t)m_renderer->GetPostProccesEffectColTex(),
                 currSize,
                 ImVec2(0, 1),
                 ImVec2(1, 0));

    m_engine->CheckProcessSceneViewInput(ImGui::IsItemHovered());


    ImGui::End();
}
