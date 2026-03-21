//
// Created by USUARIO on 1/11/2026.
//

#include "glad/glad.h"
#include "Renderer.h"

#include <iostream>
#include <variant>


#include "AABB.h"
#include "BasicShapesMeshGenerator.h"
#include "Shader.h"
#include "ModelRenderInfo.h"
#include "Scene.h"
#include "Camera.h"
#include "Engine.h"
#include "Light.h"
#include "OutlineComponent.h"
#include "ShaderManager.h"
#include "tracy/Tracy.hpp"
#include "RenderBuffer.h"


int Renderer::MAX_TEXTURE_SLOTS = 16;

namespace
{
    std::array<float, 9> blurKernel = {
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16
    };
    UniformValue blurKernelUniform = UniformValue{blurKernel};
    std::array<float, 9> edgeKernel = {
        1.0f, 1.0f, 1.0f,
        1.0f, -8.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
    };
    UniformValue edgeKernelUniform = UniformValue{edgeKernel};

    unsigned int PredefinedUBOIndexLocation(PredefinedUBO predefinedUbo)
    {
        switch (predefinedUbo)
        {
        case PredefinedUBO::CamMatrices: return 0;
        case PredefinedUBO::LightsGpu: return 1;
        default: throw std::exception("Invalid PredefinedUBO enum");
        }
    }
}

unsigned int Renderer::MAX_LIGHTS_RENDER = 1024;


Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
    m_globalOutlineInstancing = std::make_unique<OutlineComponent>();
    m_lastTimeDebugPrint = Engine::GetTime();

    TextureDesc colorTex = {
        .texFormat = TextureFormat::RGB, .texIntFormat = TextureInternalFormat::RGB_8, .mipLevels = 1,
        .uWrapping = TextureWrapping::ClampToEdge, .vWrapping = TextureWrapping::ClampToEdge,
        .width = m_width, .height = m_height, .magFilter = TextureFilter::Linear, .minFilter = TextureFilter::Linear
    };
    std::shared_ptr<Texture> colorRenderTexture = std::make_shared<Texture>(colorTex);

    RenderBufferDesc depthStencilRenderBufferDesc = {
        .intFormat = RenderBufferInternalFormat::DEPTH24_STENCIL_8, .width = m_width, .height = m_height
    };
    std::shared_ptr<RenderBuffer> depthStencilRenderBuffer = std::make_shared<RenderBuffer>(
        depthStencilRenderBufferDesc);

    std::vector<FrameBufferAttachment> colorAttachments = {
        {.attachment = colorRenderTexture, .typeAttachment = FrameBufferTypeAttachment::Color}
    };
    std::vector<FrameBufferAttachment> depthStencAttachments = {
        {.attachment = depthStencilRenderBuffer, .typeAttachment = FrameBufferTypeAttachment::DepthStencil}
    };
    m_forwardFrameBuffer = std::make_unique<FrameBuffer>(colorAttachments, depthStencAttachments);

    m_fullScreenQuad = BasicShapesMeshGenerator::CreateFullScreenQuad();

    BufferDesc camMatrixUBODesc = {
        .name = "camMatrixUbo", .type = BufferType::Constant, .storage = BufferStorage::DynamicStorage,
        .size = sizeof(CamMatrices)
    };
    m_UBOCamMatrix = std::make_unique<Buffer>();
    m_UBOCamMatrix->CreateBufferRaw(camMatrixUBODesc, nullptr);

    glBindBufferBase(GL_UNIFORM_BUFFER, PredefinedUBOIndexLocation(PredefinedUBO::CamMatrices),
                     m_UBOCamMatrix->GetBufferId());

    BufferDesc ssboLights = {
        .name = "lightsSSBO",
        .type = BufferType::Constant,
        .storage = BufferStorage::DynamicStorage,
        .size = MAX_LIGHTS_RENDER * sizeof(LightGPU)
    };
    m_SSBOLights = std::make_unique<Buffer>();
    m_SSBOLights->CreateBufferRaw(ssboLights, nullptr);

    m_lightsGpu.resize(MAX_LIGHTS_RENDER);


    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, PredefinedUBOIndexLocation(PredefinedUBO::LightsGpu),
                     m_SSBOLights->GetBufferId());
}

void Renderer::BindTexture(Texture* texture)
{
    ZoneScoped;
    if (m_currTextureSlot >= 16)
    {
        std::cout << "Run out of texture slots" << std::endl;
        return;
    }
    glBindTextureUnit(this->m_currTextureSlot, texture->GetTextureId());
    texture->SetTextureSlot(m_currTextureSlot);
    m_currTextureSlot += 1;
}

void Renderer::SortRenderItems()
{
    ZoneScoped;


    const auto& activeCamera = m_currScene->GetActiveCamera();
    for (const auto& sceneEntity : m_visibleEntities)
    {
        auto renderInfo = sceneEntity->GetModelRenderInfo();
        if (!renderInfo)
        {
            continue;
        }
        auto mesh = renderInfo->GetMesh();

        auto outlineComponent = sceneEntity->GetComponent<OutlineComponent>();

        bool hasOutlineComponent = outlineComponent;

        auto isSemiTransparent = true;

        auto submeshes = mesh->GetSubMeshes();
        auto submeshToMaterial = mesh->GetSubmeshToMaterial();


        for (unsigned int i = 0; i < mesh->GetSubMeshes().size(); i++)
        {
            auto material = submeshToMaterial[i];
            if (material->GetTransparencyType() != TransparencyType::Semitransparent)
            {
                isSemiTransparent = false;
                break;
            }
        }

        bool canUseInstancing = mesh->GetUsingInstancing() && !isSemiTransparent;


        if (canUseInstancing)
        {
            for (unsigned int i = 0; i < mesh->GetSubMeshes().size(); i++)
            {
                auto material = submeshToMaterial[i];
                DrawKey drawKey = {
                    .mesh = mesh, .material = material.get(), .submeshIndex = i,
                    .usingOutline = hasOutlineComponent
                };
                if (!m_drawKeyToInstanceItem.contains(drawKey))
                {
                    m_drawKeyToInstanceItem[drawKey] = {
                        .mesh = mesh, .material = material.get(), .transforms = {},
                        .normalMatrix = sceneEntity->GetNormalMatrix(), .submeshIndex = i,
                        .usingOutline = hasOutlineComponent

                    };
                }
                auto& instanceItem = m_drawKeyToInstanceItem[drawKey];
                instanceItem.transforms.push_back(sceneEntity->GetWorldMat());
            }
        }
        else if (isSemiTransparent)
        {
            auto entityWorldPos = sceneEntity->GetWorldPos();
            auto entitySquareDistanceToCam = (activeCamera->GetWorldPos().x - entityWorldPos.x) * (activeCamera->
                GetWorldPos().x - entityWorldPos.x) + (activeCamera->GetWorldPos().y - entityWorldPos.y) * (activeCamera
                ->GetWorldPos().y - entityWorldPos.y) + (activeCamera->GetWorldPos().z - entityWorldPos.z) * (
                activeCamera
                ->GetWorldPos().z - entityWorldPos.z);

            m_semiTransparentEntities.emplace(entitySquareDistanceToCam, sceneEntity);
        }
        else if (outlineComponent)
        {
            m_borderPassEntities.push_back(sceneEntity);
        }
        else
        {
            m_opaqueCutOutEntities.push_back(sceneEntity);
        }

        auto normalLocation = renderInfo->GetMesh()->GetVertexLayout()->GetVertexAttribLocation(
            VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::NORMAL));
        if (normalLocation != -1)
        {
            m_normalPassEntities.push_back(sceneEntity);
        }
    }
}

void Renderer::RenderScene()
{
    ZoneScoped;

    m_forwardFrameBuffer->BindFrameBuffer();
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    BindPerFrameUBOSAndSSBO();

    if (!m_currScene)
    {
        std::cout << "There is no scene assigned to the renderer" << std::endl;
        return;
    }
    FrustumCulling();

    SortRenderItems();

    BBPass();

    // Draw opaque
    for (auto entity : m_opaqueCutOutEntities)
    {
        GenericPass(entity->GetModelRenderInfo());
    }
    NormalPass();

    for (auto entity : m_borderPassEntities)
    {
        glEnable(GL_STENCIL_TEST);
        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
        GenericPass(entity->GetModelRenderInfo());
        glDisable(GL_STENCIL_TEST);
    }


    for (const auto& [drawKey, instancingDrawItem] : m_drawKeyToInstanceItem)
    {
        if (instancingDrawItem.usingOutline)
        {
            glEnable(GL_STENCIL_TEST);
            glStencilMask(0xFF);
            glStencilFunc(GL_ALWAYS, 1, 0xFF);
            glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
            instancingDrawItem.mesh->SetInstancesTransform(instancingDrawItem.transforms.data(),
                                                           instancingDrawItem.transforms.size() * sizeof(
                                                               glm::mat4));

            InstancingPass(instancingDrawItem);
            glDisable(GL_STENCIL_TEST);
        }
        else
        {
            instancingDrawItem.mesh->SetInstancesTransform(instancingDrawItem.transforms.data(),
                                                           instancingDrawItem.transforms.size() * sizeof(
                                                               glm::mat4));

            InstancingPass(instancingDrawItem);
        }
    }

    SkyboxPass();

    // Draw semitransparent
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for (auto it = m_semiTransparentEntities.rbegin(); it != m_semiTransparentEntities.rend(); ++it)
    {
        auto renderInfo = it->second->GetModelRenderInfo();
        GenericPass(renderInfo);
    }
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);


    // Draw outlines
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);

    for (auto entity : m_borderPassEntities)
    {
        auto outlineComponent = entity->GetComponent<OutlineComponent>();
        if (outlineComponent->GetSeeTrough())
        {
            glDisable(GL_DEPTH_TEST);
        }
        OutlinePass(entity->GetModelRenderInfo(), outlineComponent);
        glEnable(GL_DEPTH_TEST);
    }


    glStencilMask(0xFF);
    glDisable(GL_STENCIL_TEST);

    FullScreenQuadPass();

    PrintFrustumCullingResults();
    ClearRenderData();
}


void Renderer::PrintFrustumCullingResults()
{
    double cooldownPrint = 5.0f;

    auto currTime = Engine::GetTime();

    if (currTime > m_lastTimeDebugPrint + cooldownPrint)
    {
        std::cout << "Amount renderable entities: " << std::to_string(m_totalRenderableEntities) << std::endl;
        std::cout << "Entities that survived frustum culling: " << std::to_string(m_visibleEntities.size()) <<
            std::endl;
        m_lastTimeDebugPrint = currTime;
    }
}

void Renderer::UploadMaterialProperties(const Material* material, Shader* shader)
{
    const auto& materialProperties = material->GetMaterialProperties();
    for (int i = 0; i < materialProperties.size(); i++)
    {
        const auto materialPropertyEnum = static_cast<MaterialPropertyEnum>(i);
        const auto uniformName = Material::MaterialPropertyNameToString(materialPropertyEnum);
        auto materialProperty = materialProperties[i];
        std::visit([this](auto&& property)
        {
            using T = std::decay_t<decltype(property)>;

            if constexpr (std::is_same_v<T, Texture*>)
            {
                this->BindTexture(property);
            }
            else if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
            {
                this->BindTexture(property.get());
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


    UploadLightProperties(shader);


    for (auto& shaderProperty : activeProperties)
    {
        if (shaderProperty == ShaderBasicProperties::LightTransform && !lights.empty())
        {
            //Use first light as active camera. Change later
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightTransform = UniformValue{activeLight->GetEntity()->GetWorldMat()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightTransform);
        }
        // else if (shaderProperty == ShaderBasicProperties::ProjectionTransform && !cameras.empty())
        // {
        //     const auto& activeCamera = cameras[0]->GetComponent<Camera>();
        //     auto projMatrix = UniformValue{
        //         activeCamera->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height))
        //     };
        //     shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
        //                               projMatrix);
        // }
        // else if (shaderProperty == ShaderBasicProperties::ViewTransform && !cameras.empty())
        // {
        //     const auto& activeCamera = cameras[0]->GetComponent<Camera>();
        //     auto viewMatrix = UniformValue{activeCamera->GetViewMatrix()};
        //
        //     shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
        //                               viewMatrix);
        // }
        else if (shaderProperty == ShaderBasicProperties::LightColor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightColor = UniformValue{activeLight->m_color};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightColor);
        }
        else if (shaderProperty == ShaderBasicProperties::EyePosition && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto cameraPosition = UniformValue{activeCamera->GetEntity()->GetWorldPos()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      cameraPosition);
        }
        else if (shaderProperty == ShaderBasicProperties::LightAmbientFactor)
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightAmbientFactor = UniformValue{activeLight->m_ambientStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightAmbientFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::LightSpecularFactor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightSpecularFactor = UniformValue{activeLight->m_specularStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightSpecularFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::LightDiffuseFactor && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto activeLight = activeLightEntity->GetComponent<Light>();
            auto lightDiffuseFactor = UniformValue{activeLight->m_diffStrength};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightDiffuseFactor);
        }
        else if (shaderProperty == ShaderBasicProperties::GlobalAmbientLight)
        {
            auto m_sceneLight = m_currScene->GetSceneLight();
            auto ambientLight = UniformValue{m_sceneLight->ambientColor};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      ambientLight);
        }
        else if (shaderProperty == ShaderBasicProperties::LightPosition && !lights.empty())
        {
            auto activeLightEntity = lights[0];
            auto lightPosition = UniformValue{activeLightEntity->GetLocalPos()};

            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty),
                                      lightPosition);
        }

        else if (shaderProperty == ShaderBasicProperties::NearPlane && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto nearPlane = UniformValue{activeCamera->GetNearPlane()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty), nearPlane);
        }
        else if (shaderProperty == ShaderBasicProperties::FarPlane && !cameras.empty())
        {
            const auto& activeCamera = cameras[0]->GetComponent<Camera>();
            auto farPlane = UniformValue{activeCamera->GetFarPlane()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::FarPlane), farPlane);
        }
        else if (shaderProperty == ShaderBasicProperties::FogColor)
        {
            auto fogColor = UniformValue{m_fogColor};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty), fogColor);
        }
        else if (shaderProperty == ShaderBasicProperties::FogDensity)
        {
            auto fogDensity = UniformValue{m_fogDensity};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty), fogDensity);
        }
        else if (shaderProperty == ShaderBasicProperties::FogEffect)
        {
            auto useFog = UniformValue{m_useFog};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty), useFog);
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
        std::visit([this](auto&& property)
        {
            using T = std::decay_t<decltype(property)>;

            if constexpr (std::is_same_v<T, Texture*>)
            {
                this->BindTexture(property);
            }
            else if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
            {
                this->BindTexture(property.get());
            }
        }, uniformValue);
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

void Renderer::FrustumCulling()
{
    ZoneScoped;
    std::vector<Entity*> visibleEntities = {};
    const auto& activeCam = m_currScene->GetActiveCamera();

    auto camFrustum = CalculateFrustumCamera();


    for (const auto& sceneEntity : m_currScene->GetEntities())
    {
        auto renderInfo = sceneEntity->GetModelRenderInfo();


        if (!renderInfo)
        {
            continue;
        }
        m_totalRenderableEntities += 1;


        auto worldBoundingVol = renderInfo->GetWorldAABB();
        if (!worldBoundingVol)
        {
            continue;
        }

        if (worldBoundingVol->IsOnFrustum(camFrustum))
        {
            m_visibleEntities.push_back(sceneEntity.get());
            if (m_renderBB)
            {
                m_worldBBVolumes.push_back(sceneEntity.get());
            }
        }
    }
}

Frustum Renderer::CalculateFrustumCamera()
{
    auto camEntity = m_currScene->GetActiveCamera();
    auto activeCam = camEntity->GetComponent<Camera>();
    Frustum camFrustum;
    const float halfVSide = activeCam->GetFarPlane() * tanf(glm::radians(activeCam->GetFov()) * 0.5f);

    const float aspect = static_cast<float>(m_width) / static_cast<float>(m_height);

    const float halfHSide = halfVSide * aspect;

    auto camForward = activeCam->GetForwardCam();

    const glm::vec3 camFrontMulFar = camForward * activeCam->GetFarPlane();

    auto middleFarP = camEntity->GetWorldPos() + camFrontMulFar;
    auto rightMiddleFarP = camEntity->GetWorldPos() + camFrontMulFar + camEntity->GetRight() * halfHSide;
    auto leftMiddleFarP = camEntity->GetWorldPos() + camFrontMulFar - camEntity->GetRight() * halfHSide;
    auto upMiddleFarP = camEntity->GetWorldPos() + camFrontMulFar + camEntity->GetUp() * halfVSide;
    auto bottomBiddleFarP = camEntity->GetWorldPos() + camFrontMulFar - camEntity->GetUp() * halfVSide;

    // auto rightMiddleNearP
    // auto leftMiddleNearP
    // auto upMiddleNearP
    // auto bottomBiddleNearP


    camFrustum.m_near = Plane(camEntity->GetWorldPos() + activeCam->GetNearPlane() * camForward,
                              camForward);
    camFrustum.m_far = Plane(camEntity->GetWorldPos() + camFrontMulFar, -camForward);
    camFrustum.m_right = Plane(camEntity->GetWorldPos(),
                               glm::cross(camEntity->GetUp(), camFrontMulFar + camEntity->GetRight() * halfHSide));
    camFrustum.m_left = Plane(camEntity->GetWorldPos(),
                              glm::cross(camFrontMulFar - camEntity->GetRight() * halfHSide, camEntity->GetUp()));

    camFrustum.m_top = Plane(camEntity->GetWorldPos(),
                             glm::cross(camFrontMulFar + camEntity->GetUp() * halfVSide, camEntity->GetRight()));
    camFrustum.m_down = Plane(camEntity->GetWorldPos(),
                              glm::cross(camEntity->GetRight(), camFrontMulFar - camEntity->GetUp() * halfVSide));
    // camFrustum.m_near = Plane(camEntity->GetWorldPos() + activeCam->GetNearPlane() * camForward,
    //                           camForward);
    // camFrustum.m_far = Plane(camEntity->GetWorldPos() + camForward + camFrontMulFar, -camForward);
    // camFrustum.m_right = Plane(camEntity->GetWorldPos(),
    //                            glm::cross(camFrontMulFar + camEntity->GetRight() * halfHSide, camEntity->GetUp()));
    // camFrustum.m_left = Plane(camEntity->GetWorldPos(),
    //                           glm::cross(camEntity->GetUp(), camFrontMulFar - camEntity->GetRight() * halfHSide));
    //
    // camFrustum.m_top = Plane(camEntity->GetWorldPos(),
    //                          glm::cross(camEntity->GetRight(), camFrontMulFar - camEntity->GetUp() * halfVSide));
    // camFrustum.m_down = Plane(camEntity->GetWorldPos(),
    //                           glm::cross(camFrontMulFar + camEntity->GetUp() * halfVSide, camEntity->GetRight()));


    return camFrustum;
}

void Renderer::GenericPass(const ModelRenderInfo* renderInfo)
{
    auto mesh = renderInfo->GetMesh();


    auto& submeshes = mesh->GetSubMeshes();
    auto& submeshesMaterials = mesh->GetSubmeshToMaterial();
    glBindVertexArray(mesh->GetVao());

    for (unsigned int i = 0; i < submeshes.size(); i++)
    {
        m_currTextureSlot = 0;
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

    if (sceneLights.empty())
    {
        return;
    }

    auto amountLights = UniformValue{static_cast<int>(sceneLights.size())};
    shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::AmountLights), amountLights);

    // auto arrayLightsUniformName = Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::Lights);
    // for (int i = 0; i < sceneLights.size(); i++)
    // {
    //     auto& sceneLight = sceneLights[i];
    //     auto lightComponent = sceneLight->GetComponent<Light>();
    //
    //     auto indexString = "[" + std::to_string(i) + "].";
    //
    //     auto colorUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(LightProperty::Color);
    //     auto colorValue = UniformValue{lightComponent->m_color};
    //     shader->setUniformPerName(colorUniform, colorValue);
    //
    //     auto positionUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::Position);
    //     auto positionValue = UniformValue{sceneLight->GetLocalPos()};
    //     shader->setUniformPerName(positionUniform, positionValue);
    //
    //
    //     auto directionUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::Direction);
    //     auto directionValue = UniformValue{lightComponent->m_direction};
    //     shader->setUniformPerName(directionUniform, directionValue);
    //
    //
    //     auto ambientUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::AmbientFactor);
    //     auto ambientValue = UniformValue{lightComponent->m_ambientStrength};
    //     shader->setUniformPerName(ambientUniform, ambientValue);
    //
    //
    //     auto diffuseUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::DiffuseFactor);
    //     auto diffuseValue = UniformValue{lightComponent->m_diffStrength};
    //     shader->setUniformPerName(diffuseUniform, diffuseValue);
    //
    //
    //     auto specularUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::SpecularFactor);
    //     auto specularValue = UniformValue{lightComponent->m_specularStrength};
    //     shader->setUniformPerName(specularUniform, specularValue);
    //
    //
    //     auto innerUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::InnerConeAngle);
    //     auto innerValue = UniformValue{lightComponent->GetInnerConeAngle()};
    //     shader->setUniformPerName(innerUniform, innerValue);
    //
    //
    //     auto outerUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::OuterConeAngle);
    //     auto outerValue = UniformValue{lightComponent->GetOuterConeAngle()};
    //     shader->setUniformPerName(outerUniform, outerValue);
    //
    //
    //     auto attConstUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::AttenuationConstant);
    //     auto attConstValue = UniformValue{lightComponent->m_attenuationConstant};
    //     shader->setUniformPerName(attConstUniform, attConstValue);
    //
    //
    //     auto attLinearUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::AttenuationLinear);
    //     auto attLinearValue = UniformValue{lightComponent->m_attenuationLinear};
    //     shader->setUniformPerName(attLinearUniform, attLinearValue);
    //
    //
    //     auto attQuadraticUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::AttenuationQuadratic);
    //     auto attQuadraticValue = UniformValue{lightComponent->m_attenuationQuadratic};
    //     shader->setUniformPerName(attQuadraticUniform, attQuadraticValue);
    //
    //     auto lightTypeUniform = arrayLightsUniformName + indexString + Light::LightPropertyEnumToStr(
    //         LightProperty::LightType);
    //     auto lightTypeValue = UniformValue{static_cast<int>(lightComponent->m_lightType)};
    //
    //     shader->setUniformPerName(lightTypeUniform, lightTypeValue);
    // }
}


void Renderer::OutlinePass(ModelRenderInfo* renderInfo, OutlineComponent* outlineComponent)
{
    auto mesh = renderInfo->GetMesh();
    auto submeshes = mesh->GetSubMeshes();
    auto submeshesMaterials = mesh->GetSubmeshToMaterial();
    glBindVertexArray(mesh->GetVao());


    auto borderShader = ShaderManager::GetDefaultShader(DefaultShader::BorderColor);
    borderShader->Use();
    UploadPerFrameProperties(borderShader.get());

    auto modelTransform = renderInfo->GetEntity()->GetWorldMat();
    auto scaleMat = glm::mat4(1.0f);
    scaleMat = glm::scale(scaleMat, glm::vec3(outlineComponent->GetThickness()));
    auto finalWorldTransform = UniformValue{modelTransform * scaleMat};
    borderShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ModelTransform),
                                    finalWorldTransform);
    auto borderColor = UniformValue{outlineComponent->GetColor()};
    borderShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::BorderColor), borderColor);

    for (unsigned int i = 0; i < submeshes.size(); i++)
    {
        auto submesh = submeshes[i];

        auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
        glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset);
    }
};

void Renderer::InstancingBorderPass(DrawItemInstancing drawItemInstancing)
{
    auto mesh = drawItemInstancing.mesh;
    auto submeshes = mesh->GetSubMeshes();
    auto submeshesMaterials = mesh->GetSubmeshToMaterial();
    glBindVertexArray(mesh->GetVao());
    auto borderShader = ShaderManager::GetDefaultShader(DefaultShader::BorderColor);
    borderShader->Use();
    UploadPerFrameProperties(borderShader.get());
    auto borderColor = UniformValue{m_globalOutlineInstancing->GetColor()};
    borderShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::BorderColor), borderColor);
    auto submesh = submeshes[drawItemInstancing.submeshIndex];
    auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
    glDrawElementsInstanced(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset,
                            drawItemInstancing.transforms.size());
}

void Renderer::SetResolution(int width, int height)
{
    m_width = width;
    m_height = height;
    glViewport(0, 0, width, height);

    m_forwardFrameBuffer->ResizeColorAttachment(0, width, height);
    m_forwardFrameBuffer->ResizeDepthAttachment(width, height);
}


void Renderer::SetFaceCulling(FaceCulling faceCulling)
{
    m_faceCulling = faceCulling;

    if (m_faceCulling == FaceCulling::Disable)
    {
        glDisable(GL_CULL_FACE);
    }
    else if (m_faceCulling == FaceCulling::BackFace)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else if (m_faceCulling == FaceCulling::FrontFace)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }
    else if (m_faceCulling == FaceCulling::FrontAndBack)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT_AND_BACK);
    }
}

void Renderer::SetFaceCulling(int faceCulling)
{
    SetFaceCulling(static_cast<FaceCulling>(faceCulling));
}


void Renderer::InstancingPass(const DrawItemInstancing& instancingDrawItem)
{
    m_currTextureSlot = 0;
    auto mesh = instancingDrawItem.mesh;
    auto& submeshes = mesh->GetSubMeshes();
    auto shader = instancingDrawItem.material->GetShader();
    shader->Use();
    glBindVertexArray(mesh->GetVao());
    UploadPerFrameProperties(shader);
    UploadMaterialProperties(instancingDrawItem.material, shader);

    //UploadPerModelProperties(shader, instancingDrawItem.renderInfo);
    if (shader->GetActiveProperties().contains(ShaderBasicProperties::NormalMatrix))
    {
        auto uniformNormalMatrix = UniformValue{instancingDrawItem.normalMatrix};
        shader->setUniformPerName(
            Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::NormalMatrix), uniformNormalMatrix);
    }
    UploadLightProperties(shader);
    auto submesh = submeshes[instancingDrawItem.submeshIndex];


    auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
    glDrawElementsInstanced(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset,
                            instancingDrawItem.transforms.size());
}

void Renderer::UpdateInstancingBufferInstances()
{
    // for (const auto& [drawKey, instancingDrawItem] : m_drawKeyToInstanceItem)
    // {
    //     instancingDrawItem.mesh->SetInstancesTransform(TODO, TODO);
    // }
}

void Renderer::ClearRenderData()
{
    ZoneScoped;
    for (auto& [drawKey, instancingDrawItem] : m_drawKeyToInstanceItem)
    {
        instancingDrawItem.transforms.clear();
    }
    m_opaqueCutOutEntities.clear();
    m_semiTransparentEntities.clear();
    m_borderPassEntities.clear();
    m_visibleEntities.clear();
    m_worldBBVolumes.clear();
    m_normalPassEntities.clear();

    m_totalRenderableEntities = 0;
}

void Renderer::BBPass()
{
    if (!m_renderBB)
    {
        return;
    }
    auto aabbShader = ShaderManager::GetDefaultShader(DefaultShader::AABB);
    aabbShader->Use();

    // const auto& activeCamera = m_currScene->GetActiveCamera()->GetComponent<Camera>();
    // auto viewMatrix = UniformValue{activeCamera->GetViewMatrix()};
    // aabbShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ViewTransform), viewMatrix);
    //
    // auto projMatrix = UniformValue{
    //     activeCamera->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height))
    // };
    // aabbShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ProjectionTransform),
    //                               projMatrix);


    for (const auto& entity : m_worldBBVolumes)
    {
        auto aabbWorld = entity->GetModelRenderInfo()->GetWorldAABB();
        auto aabbCenter = UniformValue{aabbWorld->GetCenter()};
        auto aabbExtents = UniformValue{aabbWorld->GetExtents()};

        aabbShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::BBCenter), aabbCenter);
        aabbShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::AABBExtents),
                                      aabbExtents);
        glDrawArrays(GL_POINTS, 0, 1);
    }
}

void Renderer::NormalPass()
{
    if (!m_renderNormals)
    {
        return;
    }
    for (auto entity : m_normalPassEntities)
    {
        auto renderInfo = entity->GetModelRenderInfo();
        auto vertexLayout = renderInfo->GetMesh()->GetVertexLayout();
        auto normalLocation = vertexLayout->GetVertexAttribLocation(
            VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::NORMAL));


        auto normalViewerShader = ShaderManager::GetDefaultShader(DefaultShader::NormalViewer, normalLocation);

        normalViewerShader->Use();

        auto activeCam = m_currScene->GetActiveCamera()->GetComponent<Camera>();

        // auto viewMatrix = UniformValue{activeCam->GetViewMatrix()};
        // normalViewerShader->setUniformPerName(
        //     Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ViewTransform), viewMatrix);
        //
        // auto projMatrix = UniformValue{
        //     activeCam->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height))
        // };
        // normalViewerShader->setUniformPerName(
        //     Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ProjectionTransform), projMatrix);

        auto modelTransform = UniformValue{renderInfo->GetEntity()->GetWorldMat()};
        normalViewerShader->setUniformPerName(
            Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::ModelTransform), modelTransform);

        glBindVertexArray(renderInfo->GetMesh()->GetVao());

        auto& submeshes = renderInfo->GetMesh()->GetSubMeshes();
        for (unsigned int i = 0; i < submeshes.size(); i++)
        {
            auto submesh = submeshes[i];
            auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
            glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset);
        }
    }
}

void Renderer::SkyboxPass()
{
    auto skybox = m_currScene->GetSkyBox();
    if (!skybox)
    {
        return;
    }

    glDepthFunc(GL_LEQUAL);
    m_currTextureSlot = 0;

    auto skyboxMesh = skybox->GetMesh();
    auto cubeMapTexture = skybox->GetCubeMapTexture();
    BindTexture(cubeMapTexture);

    auto currCam = m_currScene->GetActiveCamera()->GetComponent<Camera>();
    auto skyboxShader = ShaderManager::GetDefaultShader(DefaultShader::Skybox);
    skyboxShader->Use();
    skyboxShader->setUniformPerName(ShaderBasicProperties::ViewTransform,
                                    glm::mat4(glm::mat3(currCam->GetViewMatrix())));
    skyboxShader->setUniformPerName(ShaderBasicProperties::ProjectionTransform,
                                    currCam->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height)));
    skyboxShader->setUniformPerName(ShaderBasicProperties::SkyBox, cubeMapTexture);

    glBindVertexArray(skyboxMesh->GetVao());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    glDepthFunc(GL_LESS);
}


void Renderer::FullScreenQuadPass()
{
    m_currTextureSlot = 0;
    auto renderedSceneTex = m_forwardFrameBuffer->GetColorAttachmentTexture(0);
    if (!renderedSceneTex)
    {
        std::cerr << "No rendered scene texture in the forward pass frame buffer" << std::endl;
        return;
    }

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    auto fullScreenQuadShader = ShaderManager::GetDefaultShader(DefaultShader::FullScreenQuad);
    fullScreenQuadShader->Use();
    BindTexture(renderedSceneTex);
    auto uniformRenderedSceneTex = UniformValue{renderedSceneTex};
    fullScreenQuadShader->setUniformPerName(Shader::ShaderPropertyEnumToStr(ShaderBasicProperties::RenderedScene),
                                            uniformRenderedSceneTex);

    bool usingPostProcessEffect = false;
    if (m_postProcessEffect == PostProcessEffect::Blur)
    {
        fullScreenQuadShader->setUniformPerName(ShaderBasicProperties::Kernel,
                                                blurKernelUniform);

        usingPostProcessEffect = true;
    }
    else if (m_postProcessEffect == PostProcessEffect::Edge)
    {
        fullScreenQuadShader->setUniformPerName(ShaderBasicProperties::Kernel,
                                                edgeKernelUniform);

        usingPostProcessEffect = true;
    }

    fullScreenQuadShader->setUniformPerName(ShaderBasicProperties::ApplyKernel, usingPostProcessEffect);
    if (usingPostProcessEffect)
    {
        fullScreenQuadShader->setUniformPerName(ShaderBasicProperties::RenderedSceneWidth, m_width);
        fullScreenQuadShader->setUniformPerName(ShaderBasicProperties::RenderedSceneHeight, m_height);
    }


    glBindVertexArray(m_fullScreenQuad->GetVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glEnable(GL_DEPTH_TEST);
    SetFaceCulling(m_faceCulling);
}

void Renderer::BindPerFrameUBOSAndSSBO()
{
    auto currCam = m_currScene->GetActiveCamera()->GetComponent<Camera>();

    CamMatrices camMatrix = {
        .viewMatrix = currCam->GetViewMatrix(),
        .projMatrix = currCam->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height)),
    };
    m_UBOCamMatrix->BufferUploadData(&camMatrix, 0, sizeof(CamMatrices));


    //Upload lights to UBO

    const auto& sceneLights = m_currScene->GetLights();

    if (!sceneLights.empty())
    {
        for (int i = 0; i < MAX_LIGHTS_RENDER; i++)
        {
            if (i >= sceneLights.size())
            {
                break;
            }
            auto light = sceneLights[i]->GetComponent<Light>();


            m_lightsGpu[i] = light->GetLightGPU();
        }
    }

    m_SSBOLights->BufferUploadData(m_lightsGpu.data(), 0, sceneLights.size() * sizeof(LightGPU));
}
