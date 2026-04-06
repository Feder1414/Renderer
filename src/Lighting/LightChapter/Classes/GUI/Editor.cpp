//
// Created by USUARIO on 2/8/2026.
//

#include "Editor.h"
#include "imgui.h"
#include <memory>
#include "Engine.h"
#include "imgui_internal.h"
#include "tracy/Tracy.hpp"

Editor::Editor(Engine* engine) : IWidget("Editor")
{
    m_engine = engine;
    m_entityTree = std::make_unique<EntityTree>(m_engine->GetCurrScene());
    m_entityTree->onEntitySelected.AddListener([this](Entity* entity)
    {
        OnEntitySelectedEntityTree(entity);
    });

    m_componentInspector = std::make_unique<ComponentInspector>();
    m_componentInspector->SetScene(m_engine->GetCurrScene());
    m_generalRenderSettings = std::make_unique<GeneralRendererSettingsComponent>(m_engine->GetRenderer());
    m_sceneView = std::make_unique<SceneView>(engine->GetRenderer(), engine);
}


void Editor::Render()
{
    ZoneScoped;
    // if (!ImGui::Begin(m_windowName.c_str()))
    // {
    //     ImGui::End();
    //     return;
    // }

    ImGuiID dockSpaceId = ImGui::GetID(m_windowName.c_str());
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    // Create settings
    if (!m_initialized)
    {
        ImGui::DockBuilderAddNode(dockSpaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockSpaceId, viewport->Size);
        ImGuiID dockIdLeft = 0;
        ImGuiID dockIdMain = dockSpaceId;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Left, 0.20f, &dockIdLeft, &dockIdMain);


        ImGuiID dockIdRight = 0;
        ImGui::DockBuilderSplitNode(dockIdMain, ImGuiDir_Right, 0.20, &dockIdRight, &dockIdMain);
        ImGuiID dockIdTopLeft = 0;
        ImGuiID dockIdBottomLeft = 0;
        ImGui::DockBuilderSplitNode(dockIdLeft, ImGuiDir_Up, 0.60, &dockIdTopLeft, &dockIdBottomLeft);


        ImGui::DockBuilderDockWindow(m_sceneView->GetWindowName().c_str(), dockIdMain);
        ImGui::DockBuilderDockWindow(m_entityTree->GetWindowName().c_str(), dockIdTopLeft);
        ImGui::DockBuilderDockWindow(m_generalRenderSettings->GetWindowName().c_str(), dockIdBottomLeft);
        ImGui::DockBuilderDockWindow(m_componentInspector->GetWindowName().c_str(), dockIdRight);
        ImGui::DockBuilderFinish(dockSpaceId);
        m_initialized = true;
    }

    ImGui::DockSpaceOverViewport(dockSpaceId, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
    m_sceneView->Render();
    m_generalRenderSettings->Render();
    m_entityTree->Render();
    m_componentInspector->Render();


    // ImGui::End();
}

void Editor::OnEntitySelectedEntityTree(Entity* entity)
{
    m_componentInspector->SetEntity(entity);
}
