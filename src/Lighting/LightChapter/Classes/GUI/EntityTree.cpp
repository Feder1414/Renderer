//
// Created by USUARIO on 2/8/2026.
//

#include "EntityTree.h"

#include "imgui.h"
#include "Scene.h"

void EntityTree::Render()
{
    auto& sceneTree = m_scene->GetSceneTree();
    if (ImGui::BeginChild("treePanel", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_ResizeY,
                          ImGuiChildFlags_NavFlattened))

    {
        ImGui::SetNextItemWidth(-FLT_MIN);

        if (ImGui::BeginTable("tableTree", 1, ImGuiTableFlags_RowBg))
        {
            for (auto& entity : sceneTree)
            {
                DrawNodeEntity(entity);
            }
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
}

void EntityTree::DrawNodeEntity(Entity* entity)
{
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushID(entity->GetIdStr().c_str());
    ImGuiTreeNodeFlags tree_flags = ImGuiTreeNodeFlags_None;
    tree_flags |= ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    tree_flags |= ImGuiTreeNodeFlags_DrawLinesToNodes;

    if (entity == m_focusedEntity)
    {
        tree_flags |= ImGuiTreeNodeFlags_Selected;
    }
    if (entity->GetChild().empty())
    {
        tree_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
    }
    bool node_open = ImGui::TreeNodeEx("", tree_flags, "%s", entity->GetName().c_str());
    if (ImGui::IsItemFocused() && m_focusedEntity != entity)
    {
        m_focusedEntity = entity;
        onEntitySelected.Notify(entity);
    }
    if (node_open)
    {
        for (auto& childEntity : entity->GetChild())
        {
            DrawNodeEntity(childEntity);
        }
        ImGui::TreePop();
    }
    ImGui::PopID();
}
