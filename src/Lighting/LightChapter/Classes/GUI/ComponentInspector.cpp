//
// Created by USUARIO on 2/9/2026.
//

#include "ComponentInspector.h"

#include <iostream>

#include "Entity.h"
#include "imgui.h"
#include "../../Components/IComponent.h"
#include "../../Components/Metadata/ComponentProperty.h"
#include "Metadata/GlobalEnumMetadata.h"


void ComponentInspector::Render()
{
    if (!m_entity)
    {
        return;
    }
    if (!ImGui::Begin("Component inspector"))
    {
        ImGui::End();
        return;
    }
    if (ImGui::BeginChild("Inspector"))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);

        RenderTransformComponenent();

        for (auto& component : m_entity->GetComponents())
        {
            RenderGenericComponent(component.get());
        }

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();


    ImGui::End();
}

void ComponentInspector::RenderTransformComponenent()
{
    if (!ImGui::CollapsingHeader("Transform component"))
    {
        return;
    }
    SetTransformComponent();

    if (ImGui::BeginChild("Component transform editor"))
    {
        ImGui::TextUnformatted("Position");

        ImGui::PushItemWidth(80);

        //Sliders pos
        ImGui::TextUnformatted("X");
        ImGui::SameLine();
        if (ImGui::DragFloat("## posx", &m_pos[0], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalPos(m_pos);
        }


        ImGui::SameLine();
        ImGui::TextUnformatted("Y");
        ImGui::SameLine();
        if (ImGui::DragFloat("## posy", &m_pos[1], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalPos(m_pos);
        };


        ImGui::SameLine();
        ImGui::TextUnformatted("Z");
        ImGui::SameLine();
        if (ImGui::DragFloat("## posz", &m_pos[2], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalPos(m_pos);
        }


        //Sliders rot
        ImGui::TextUnformatted("X");
        ImGui::SameLine();
        if (ImGui::DragFloat("## rotx", &m_rot[0], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalRot(m_rot);
        }


        ImGui::SameLine();
        ImGui::TextUnformatted("Y");
        ImGui::SameLine();
        if (ImGui::DragFloat("## roty", &m_rot[1], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalRot(m_rot);
        }


        ImGui::SameLine();
        ImGui::TextUnformatted("Z");
        ImGui::SameLine();
        if (ImGui::DragFloat("## rotz", &m_rot[2], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalRot(m_rot);
        }

        //Sliders scale
        ImGui::TextUnformatted("X");
        ImGui::SameLine();
        if (ImGui::DragFloat("## scalex", &m_scale[0], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalScale(m_scale);
        }


        ImGui::SameLine();
        ImGui::TextUnformatted("Y");
        ImGui::SameLine();
        if (ImGui::DragFloat("## scaley", &m_scale[1], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalScale(m_scale);
        }


        ImGui::SameLine();
        ImGui::TextUnformatted("Z");
        ImGui::SameLine();
        if (ImGui::DragFloat("## scalez", &m_scale[2], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
        {
            m_entity->SetLocalScale(m_scale);
        }
    }
    ImGui::EndChild();
}

void ComponentInspector::SetEntity(Entity* entity)
{
    m_entity = entity;
    m_genericComponentValues.clear();
    SetTransformComponent();
}

void ComponentInspector::RenderGenericComponent(IComponent* component)
{
    if (!ImGui::CollapsingHeader(component->GetComponentName().c_str()))
    {
        return;
    }

    auto componentMetadata = component->GetComponentMetadata();

    if (componentMetadata.empty())
    {
        return;
    }

    ImGui::PushID(component->GetComponentName().c_str());
    if (!m_genericComponentValues.contains(component->GetComponentName()))
    {
        m_genericComponentValues[component->GetComponentName()] = {};
    }

    auto& currComponentMetadata = m_genericComponentValues[component->GetComponentName()];
    auto& componentSetterGetter = component->GetSetterGetter();


    for (auto& pair : componentMetadata)
    {
        auto propertyName = pair.first;
        auto& propertyMetadata = pair.second;

        const auto& propertySetterGetter = componentSetterGetter.find(propertyName);
        if (propertySetterGetter == componentSetterGetter.end())
        {
            continue;
        }
        ImGui::PushID(propertyName.c_str());

        auto propertyValue = propertySetterGetter->second.GetGetter()();


        if (!currComponentMetadata.contains(propertyName))
        {
            currComponentMetadata[propertyName] = propertyValue;
        }

        auto& propertyInspectorVal = currComponentMetadata[propertyName];

        ImGui::TextUnformatted(propertyName.c_str());
        ImGui::SameLine();
        switch (propertyMetadata.GetPropertyType())
        {
        case PropertyType::BOOL:
            {
                auto boolean = std::any_cast<bool>(propertyInspectorVal);

                if (ImGui::Checkbox("##combo:bool", &boolean))
                {
                    propertySetterGetter->second.GetSetter()(boolean);
                }
                propertyInspectorVal = boolean;
                break;
            }


        case PropertyType::COLOR:
            {
                auto color = std::any_cast<glm::vec3>(propertyInspectorVal);
                if (ImGui::ColorPicker3("##ColorPicker", &color[0],
                                        ImGuiColorEditFlags_PickerHueBar | ImGuiColorEditFlags_NoSidePreview |
                                        ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha))
                {
                    propertySetterGetter->second.GetSetter()(color);
                };
                propertyInspectorVal = color;
                break;
            }


        case PropertyType::DOUBLE:
            {
                // auto floatVal = std::any_cast<double>(propertyInspectorVal);
                // if (ImGui::DragFloat("##ColorPicker", &floatVal, 0.005, -FLT_MAX, FLT_MAX, "&.3f"))
                // {
                //     propertySetterGetter->second.GetSetter()(floatVal);
                // };
                // propertyInspectorVal = floatVal;
                break;
            }


        case PropertyType::ENUM:
            {
                auto enumVal = std::any_cast<int>(propertyValue);
                auto enumName = propertyMetadata.GetEnumName();
                auto enumMetadata = GlobalEnumMetadata::GetEnumMetadata(enumName);
                auto enumNames = enumMetadata->GetNames();
                auto enumValues = enumMetadata->GetValues();
                if (ImGui::BeginCombo("", enumMetadata->GetNames()[0].c_str(), 0))
                {
                    for (int i = 0; i < enumMetadata->GetNames().size(); i++)
                    {
                        const bool isSelected = (enumVal == enumValues[i]);
                        if (ImGui::Selectable(enumNames[i].c_str(), isSelected))
                        {
                            enumVal = enumValues[i];
                            propertySetterGetter->second.GetSetter()(enumVal);
                        }
                        if (isSelected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                propertyValue = enumVal;

                break;
            }

        case PropertyType::FLOAT:
            {
                auto floatVal = std::any_cast<float>(propertyInspectorVal);
                if (ImGui::DragFloat("##ColorPicker", &floatVal, 0.005, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    propertySetterGetter->second.GetSetter()(floatVal);
                };
                propertyInspectorVal = floatVal;
                break;
            }


        case PropertyType::INT:
            {
                auto intVal = std::any_cast<int>(propertyInspectorVal);
                if (ImGui::DragInt("##ColorPicker", &intVal, 1.0f, -INT_MAX, INT_MAX))
                {
                    propertySetterGetter->second.GetSetter()(intVal);
                }
                propertyInspectorVal = intVal;
                break;
            }

        case PropertyType::VEC3:
            {
                auto vec3 = std::any_cast<glm::vec3>(propertyInspectorVal);
                if (ImGui::DragFloat3("Float3", &vec3[0], 0.005f, -FLT_MAX, FLT_MAX, "%.3f"))
                {
                    propertySetterGetter->second.GetSetter()(vec3);
                }
                propertyInspectorVal = vec3;
            }


            break;
        }
        ImGui::PopID();
    }
    ImGui::PopID();
}


void ComponentInspector::SetTransformComponent()
{
    auto entityPos = m_entity->GetLocalPos();
    m_pos = entityPos;


    auto entityRot = m_entity->GetLocalRot();
    m_rot = entityRot;

    auto entityScale = m_entity->GetLocalScale();
    m_scale = entityScale;
}

void ComponentInspector::UpdateEntityTransform(TransformUpdated transformUpdated)
{
}

