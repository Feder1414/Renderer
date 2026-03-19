//
// Created by USUARIO on 2/8/2026.
//

#include "Editor.h"
#include "imgui.h"
#include <memory>
#include "Engine.h"
#include "tracy/Tracy.hpp"

Editor::Editor(Engine* engine)
{
    m_engine = engine;
    m_entityTree = std::make_unique<EntityTree>(m_engine->GetCurrScene());
    m_entityTree->onEntitySelected.AddListener([this](Entity* entity)
    {
        OnEntitySelectedEntityTree(entity);
    });

    m_componentInspector = std::make_unique<ComponentInspector>();
    m_generalRenderSettings = std::make_unique<GeneralRendererSettingsComponent>(m_engine->GetRenderer());
}


void Editor::Render()
{
    ZoneScoped;
    if (!ImGui::Begin("Editor"))
    {
        ImGui::End();
        return;
    }

    m_generalRenderSettings->Render();
    m_entityTree->Render();
    m_componentInspector->Render();
    ImGui::End();
}

void Editor::OnEntitySelectedEntityTree(Entity* entity)
{
    m_componentInspector->SetEntity(entity);
}
