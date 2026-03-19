//
// Created by USUARIO on 3/6/2026.
//

#include "GeneralRendererSettingsComponent.h"

#include "imgui.h"
#include "Renderer.h"

#define X(name, value) #name

namespace
{
    std::vector<std::string> faceCullingNames = {ENUM_FACE_CULLING(X)};
    std::vector<std::string> postProcessEffectNames = {ENUM_FACE_POST_PROCESS_EFFECT(X)};

    int GetSelectedIndexEnum(int enumVal, const std::vector<std::string>& enumStringVal)
    {
        int selectedIndex = 0;
        for (int i = 0; i < enumStringVal.size(); i++)
        {
            if (i == enumVal)
            {
                selectedIndex = enumVal;
                break;
            }
        }
        return selectedIndex;
    }
}
#undef X

void GeneralRendererSettingsComponent::Render()
{
    if (!m_renderer)
    {
        return;
    }
    if (!ImGui::Begin("General renderer settings"))
    {
        ImGui::End();
        return;
    }

    if (ImGui::BeginChild("General render settings"))
    {
        bool renderBB = m_renderer->GetIsRenderingBB();
        if (ImGui::Checkbox("Render     bounding volumes", &renderBB))
        {
            m_renderer->SetRenderBB(renderBB);

            //
        }

        bool renderNormals = m_renderer->GetIsRenderingNormals();
        if (ImGui::Checkbox("Render normals", &renderNormals))
        {
            m_renderer->SetRenderNormals(renderNormals);
            //
        }

        FaceCulling faceCulling = m_renderer->GetFaceCulling();


        int faceCullingIndexSelected = GetSelectedIndexEnum(static_cast<int>(faceCulling), faceCullingNames);
        if (ImGui::BeginCombo("Face culling", faceCullingNames[faceCullingIndexSelected].c_str(), 0))
        {
            for (int i = 0; i < faceCullingNames.size(); i++)
            {
                const bool isSelected = i == faceCullingIndexSelected;
                if (ImGui::Selectable(faceCullingNames[i].c_str(), isSelected))
                {
                    m_renderer->SetFaceCulling(i);
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        PostProcessEffect postProcessEffect = m_renderer->GetPostProcessEffect();

        auto postProcessIndexSelected = GetSelectedIndexEnum(static_cast<int>(postProcessEffect),
                                                             postProcessEffectNames);

        if (ImGui::BeginCombo("Post process effect", postProcessEffectNames[postProcessIndexSelected].c_str(), 0))
        {
            for (int i = 0; i < postProcessEffectNames.size(); i++)
            {
                const bool isSelected = i == postProcessIndexSelected;
                if (ImGui::Selectable(postProcessEffectNames[i].c_str(), isSelected))
                {
                    m_renderer->SetPostProcessEffect(static_cast<PostProcessEffect>(i));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }


        //
    }

    ImGui::EndChild();

    ImGui::End();
}
