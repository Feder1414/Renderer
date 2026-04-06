//
// Created by USUARIO on 3/6/2026.
//

#include "GeneralRendererSettingsComponent.h"

#include "imgui.h"
#include "Renderer.h"

#define X(name, value) #name

namespace
{
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
    // ReSharper disable once CppDFAUnreachableCode

    if (!ImGui::Begin(m_windowName.c_str()))
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
        const auto& faceCullingNames = m_renderer->GetFaceCullingNames();


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
        auto postProcessEffectNames = m_renderer->GetPostProcessEffectNames();

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


        auto msaaNames = m_renderer->GetMSAANames();
        auto currMSAAMode = m_renderer->GetCurrMsaaMode();

        bool useMSAA = m_renderer->GetUsingMSAA();
        if (ImGui::Checkbox("Use MSAA", &useMSAA))
        {
            m_renderer->SetUsingMSAA(useMSAA);
        }

        if (ImGui::BeginCombo("Antialiasing MSAA", msaaNames[static_cast<int>(currMSAAMode)].c_str(), 0))
        {
            for (int i = 0; i < msaaNames.size(); i++)
            {
                const bool isSelected = i == static_cast<int>(currMSAAMode);
                if (ImGui::Selectable(msaaNames[i].c_str(), isSelected))
                {
                    m_renderer->SetMSAAMode(static_cast<MSAA>(i));
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }

            ImGui::EndCombo();
        }

        bool usingGammaCorrection = m_renderer->GetUsingGammaCorrection();
        if (ImGui::Checkbox("Use gamma correction", &usingGammaCorrection))
        {
            m_renderer->SetUsingGammaCorrection(usingGammaCorrection);
        }

        if (ImGui::Button("See shadow map"))
        {
            m_seeShadowMap = !m_seeShadowMap;
            m_renderer->SetDebugShadowPass(m_seeShadowMap);
        }

        if (m_seeShadowMap)
        {
            m_shadowMapViewer->Render();
        }


        //
    }


    ImGui::EndChild();

    ImGui::End();
}
