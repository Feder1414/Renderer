//
// Created by USUARIO on 1/11/2026.
//

#include "Renderer.h"

#include <iostream>
#include <variant>


#include "OpenGL.h"
#include "Shader.h"
#include "ModelRenderInfo.h"
#include "Scene.h"
#include "Camera.h"


namespace
{
    void DebugMatrix(glm::mat3 mat)
    {
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                std::cout << mat[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
}

void Renderer::RenderScene()
{
    if (!m_currScene)
    {
        std::cout << "There is no scene assigned to the renderer" << std::endl;
        return;
    }
    const auto& sceneEntities = m_currScene->GetEntities();
    for (const auto& sceneEntity : sceneEntities)
    {
        auto renderInfo = sceneEntity->GetModelRenderInfo();
        if (renderInfo)
        {
            DrawModel(renderInfo);
        }
    }

    if (m_renderLights)
    {
        auto const& lights = m_currScene->GetLights();
        for (auto& light : lights)
        {
            auto renderInfo = light->GetModelRenderInfo();
            if (renderInfo)
            {
                DrawModel(renderInfo);
            }
        }
    }
}

void Renderer::UploadMaterialProperties(const Material* material, Shader* shader)
{
    int indexTextureSlot = 0;

    const auto& materialProperties = material->GetMaterialProperties();
    for (int i = 0; i < materialProperties.size(); i++)
    {
        const auto materialPropertyEnum = static_cast<MaterialPropertyEnum>(i);
        const auto uniformName = Material::MaterialPropertyNameToString(materialPropertyEnum);
        auto materialProperty = materialProperties[i];
        std::visit([&indexTextureSlot](auto&& property)
        {
            using T = std::decay_t<decltype(property)>;
            if constexpr (std::is_same_v<T, Texture*>)
            {
                glActiveTexture(GL_TEXTURE0 + indexTextureSlot);
                glBindTexture(GL_TEXTURE_2D, property->GetTextureId());
                indexTextureSlot += 1;
            }
        }, materialProperty);
        shader->setUniformPerName(uniformName, materialProperty);
    }

    // Upload material dynamic
    const auto& dynamicPropertiesMaterial = material->GetShaderUniformValues();
    this->UploadUniformProperties(dynamicPropertiesMaterial, shader);
}

void Renderer::UploadPerFrameProperties(Shader* shader)
{
    const auto& activeProperties = shader->GetActiveProperties();
    const auto& lights = m_currScene->GetLights();
    const auto& cameras = m_currScene->GetCameras();


    for (auto& shaderProperty : activeProperties)
    {
        if (shaderProperty == ShaderBasicProperties::LightTransform && !lights.empty())
        {
            //Use first light as active camera. Change later
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightTransform = UniformValue{activeLight->GetEntity()->GetWorldMat()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightTransform),
                                      lightTransform);
        }
        else if (shaderProperty == ShaderBasicProperties::ProjectionTransform && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto projMatrix = UniformValue{activeCamera->GetProjMatrix(m_width / m_height)};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ProjectionTransform),
                                      projMatrix);
        }
        else if (shaderProperty == ShaderBasicProperties::ViewTransform && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto viewMatrix = UniformValue{activeCamera->GetViewMatrix()};

            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ViewTransform),
                                      viewMatrix);
        }
        else if (shaderProperty == ShaderBasicProperties::LightColor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightColor = UniformValue{activeLight->m_color};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightColor),
                                      lightColor);
        }
        else if (shaderProperty == ShaderBasicProperties::EyePosition && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto cameraPosition = UniformValue{activeCamera->GetTransform().m_position};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::EyePosition),
                                      cameraPosition);
        }
        else if (shaderProperty == ShaderBasicProperties::LightAmbientFactor)
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightAmbientFactor = UniformValue{activeLight->m_ambientStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightAmbientFactor),
                                      lightAmbientFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::LightSpecularFactor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightSpecularFactor = UniformValue{activeLight->m_specularStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightSpecularFactor),
                                      lightSpecularFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::LightDiffuseFactor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightDiffuseFactor = UniformValue{activeLight->m_diffStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightDiffuseFactor),
                                      lightDiffuseFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::GlobalAmbientLight)
        {
            auto m_sceneLight = m_currScene->GetSceneLight();
            auto ambientLight = UniformValue{m_sceneLight->ambientColor};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::GlobalAmbientLight),
                                      ambientLight);
        }
        else if (shaderProperty == ShaderBasicProperties::LightPosition && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto lightPosition = UniformValue{activeLightEntity->GetLocalPos()};

            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::LightPosition),
                                      lightPosition);
        }
        else if (shaderProperty == ShaderBasicProperties::Lights && !lights.empty())
        {
            UploadLightProperties(shader);
        }
    }
}


void Renderer::UploadUniformProperties(const std::unordered_map<std::string, UniformValue>& uniformValues,
                                       Shader* shader)
{
    for (auto& kp : uniformValues)
    {
        auto uniformName = kp.first;
        auto uniformValue = kp.second;
        shader->setUniformPerName(uniformName, uniformValue);
    }
}

void Renderer::UploadPerModelProperties(Shader* shader, const ModelRenderInfo* object)
{
    const auto& activeShaderProperties = shader->GetActiveProperties();
    auto entity = object->GetEntity();

    if (activeShaderProperties.contains(ShaderBasicProperties::ModelTransform))
    {
        auto modelTransformMatrix = UniformValue{entity->GetWorldMat()};
        shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ModelTransform),
                                  modelTransformMatrix);
    }
    if (activeShaderProperties.contains(ShaderBasicProperties::NormalMatrix))
    {
        auto normalMatrix = entity->GetNormalMatrix();
        auto uniformNormalMatrix = UniformValue{normalMatrix};

        shader->setUniformPerName(
            Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::NormalMatrix), uniformNormalMatrix);
    }

    for (const auto& kp : object->GetUniformValuesInstances())
    {
        auto uniformName = kp.first;
        auto value = kp.second;
        shader->setUniformPerName(uniformName, value);
    }
}

void Renderer::DrawModel(const ModelRenderInfo* renderInfo)
{
    auto mesh = renderInfo->GetMesh();
    auto modelBuffers = mesh->GetBuffers();

    auto submeshes = mesh->GetSubMeshes();
    auto submeshesMaterials = mesh->GetSubmeshToMaterial();
    glBindVertexArray(modelBuffers.vao);
    for (unsigned int i = 0; i < submeshes.size(); i++)
    {
        auto submeshMaterial = submeshesMaterials[i];
        auto submesh = submeshes[i];
        auto objectShader = submeshMaterial->GetShader();
        objectShader->Use();
        UploadPerFrameProperties(objectShader);
        UploadMaterialProperties(submeshMaterial.get(), objectShader);
        UploadPerModelProperties(objectShader, renderInfo);
        UploadLightProperties(objectShader);


        auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
        glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset);
    }
}


void Renderer::ChangeResolution(float width, float height)
{
    m_width = width;
    m_height = height;
}

void Renderer::UploadLightProperties(Shader* shader)
{
    const auto& sceneLights = m_currScene->GetLights();

    auto amountLights = UniformValue{static_cast<int>(sceneLights.size())};
    shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::AmountLights), amountLights);

    auto arrayLightsUniformName = Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::Lights);
    for (int i = 0; i < sceneLights.size(); i++)
    {
        auto& sceneLight = sceneLights[i];
        auto lightComponent = sceneLight->GetComponent<Light>();

        auto indexString = "[" + std::to_string(i) + "].";

        auto colorUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(LightProperty::Color);
        auto colorValue = UniformValue{lightComponent->m_color};
        shader->setUniformPerName(colorUniform, colorValue);

        auto positionUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::Position);
        auto positionValue = UniformValue{sceneLight->GetLocalPos()};
        shader->setUniformPerName(positionUniform, positionValue);


        auto directionUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::Direction);
        auto directionValue = UniformValue{lightComponent->m_direction};
        shader->setUniformPerName(directionUniform, directionValue);


        auto ambientUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::AmbientFactor);
        auto ambientValue = UniformValue{lightComponent->m_ambientStrength};
        shader->setUniformPerName(ambientUniform, ambientValue);


        auto diffuseUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::DiffuseFactor);
        auto diffuseValue = UniformValue{lightComponent->m_diffStrength};
        shader->setUniformPerName(diffuseUniform, diffuseValue);


        auto specularUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::SpecularFactor);
        auto specularValue = UniformValue{lightComponent->m_specularStrength};
        shader->setUniformPerName(specularUniform, specularValue);


        auto innerUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::InnerConeAngle);
        auto innerValue = UniformValue{lightComponent->GetInnerConeAngle()};
        shader->setUniformPerName(innerUniform, innerValue);


        auto outerUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::OuterConeAngle);
        auto outerValue = UniformValue{lightComponent->GetOuterConeAngle()};
        shader->setUniformPerName(outerUniform, outerValue);


        auto attConstUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::AttenuationConstant);
        auto attConstValue = UniformValue{lightComponent->m_attenuationConstant};
        shader->setUniformPerName(attConstUniform, attConstValue);


        auto attLinearUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::AttenuationLinear);
        auto attLinearValue = UniformValue{lightComponent->m_attenuationLinear};
        shader->setUniformPerName(attLinearUniform, attLinearValue);


        auto attQuadraticUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::AttenuationQuadratic);
        auto attQuadraticValue = UniformValue{lightComponent->m_attenuationQuadratic};
        shader->setUniformPerName(attQuadraticUniform, attQuadraticValue);

        auto lightTypeUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
            LightProperty::LightType);
        auto lightTypeValue = UniformValue{static_cast<int>(lightComponent->m_lightType)};

        shader->setUniformPerName(lightTypeUniform, lightTypeValue);
    }
}
