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
        case PredefinedUBO::LightMatrices: return 2;
        default: throw std::exception("Invalid PredefinedUBO enum");
        }
    }
}

unsigned int Renderer::MAX_LIGHTS_RENDER = 1024;


Renderer::Renderer(int width, int height) : m_width(width), m_height(height)
{
    m_globalOutlineInstancing = std::make_unique<OutlineComponent>();
    m_lastTimeDebugPrint = Engine::GetTime();


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

    BufferDesc descUBOLightMatrix = {
        .name = "Lights matrix",
        .type = BufferType::Constant,
        .storage = BufferStorage::DynamicStorage,
        .size = sizeof(CamMatrices)
    };
    m_UBOLightMatrix = std::make_unique<Buffer>();
    m_UBOLightMatrix->CreateBufferRaw(descUBOLightMatrix, nullptr);
    glBindBufferBase(GL_UNIFORM_BUFFER, PredefinedUBOIndexLocation(PredefinedUBO::LightMatrices),
                     m_UBOLightMatrix->GetBufferId());


    SetUsingMSAA(m_usingMSAA);
    InitializeFrameBuffers();
}

void Renderer::SetScene(Scene* scene)
{
    m_currScene = scene;

    auto camera = m_currScene->GetActiveCamera()->GetComponent<Camera>();

    m_sceneViewport = std::make_unique<ViewPort>(1, 1, camera, m_postProcessFrameBuffer.get());

    auto currDirLight = scene->GetCurrDirLight()->GetComponent<Light>();

    m_shadowViewPort = std::make_unique<ViewPort>(1024, 1024, currDirLight, m_shadowFrameBuffer.get());
    m_shadowDebugViewPort = std::make_unique<ViewPort>(1, 1, currDirLight, m_shadowFrameBufferDebug.get());
}


void Renderer::InitializeFrameBuffers()
{
    // Initialize forward render buffer
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
    m_forwardFrameBuffer = std::make_unique<FrameBuffer>(colorAttachments, depthStencAttachments, m_width, m_height);
    m_forwardFrameBuffer->SetName("Forward framebuffer");


    //Initialize post process frame buffer
    TextureDesc colorTexSRGB = {
        .texFormat = TextureFormat::RGB, .texIntFormat = TextureInternalFormat::SRGB8, .mipLevels = 1,
        .uWrapping = TextureWrapping::ClampToEdge, .vWrapping = TextureWrapping::ClampToEdge,
        .width = m_width, .height = m_height, .magFilter = TextureFilter::Linear, .minFilter = TextureFilter::Linear
    };
    auto colorTexPostProcess = std::make_shared<Texture>(colorTexSRGB);
    std::vector<FrameBufferAttachment> postProcessEffectsColorAttachments = {
        {.attachment = colorTexPostProcess, .typeAttachment = FrameBufferTypeAttachment::Color}
    };
    std::vector<FrameBufferAttachment> emptyDepthStencAttach = {};
    m_postProcessFrameBuffer = std::make_unique<FrameBuffer>(postProcessEffectsColorAttachments, emptyDepthStencAttach,
                                                             m_width, m_height);
    m_postProcessFrameBuffer->SetName("Post process frame buffer");


    //Frame buffer for shadow mapping
    TextureDesc depthTexDesc = {
        .texFormat = TextureFormat::DEPTH, .texIntFormat = TextureInternalFormat::DEPTH24, .mipLevels = 1,
        .uWrapping = TextureWrapping::ClampToBorder, .vWrapping = TextureWrapping::ClampToBorder,
        .width = 1024, .height = 1024, .magFilter = TextureFilter::Linear, .minFilter = TextureFilter::Linear
    };
    auto depthTex = std::make_shared<Texture>(depthTexDesc);


    std::vector<FrameBufferAttachment> depthShadowAttachment = {
        {.attachment = depthTex, .typeAttachment = FrameBufferTypeAttachment::Depth}
    };
    std::vector<FrameBufferAttachment> colorShadowAttachments = {};
    m_shadowFrameBuffer = std::make_unique<FrameBuffer>(colorShadowAttachments, depthShadowAttachment, 1024,
                                                        1024);
    m_shadowFrameBuffer->SetDrawColorAttachment(-1);
    m_shadowFrameBuffer->SetReadColorAttachment(-1);
    m_shadowFrameBuffer->SetName("ShadowFrameBuffer");


    // Initialize MSAA render buffers

    for (int i = 0; i < m_msaaFB.size(); i++)
    {
        unsigned int amountSamples = 2;
        if (static_cast<MSAA>(i) == MSAA::SAMPLE2)
        {
            amountSamples = 2;
        }
        else if (static_cast<MSAA>(i) == MSAA::SAMPLE4)
        {
            amountSamples = 4;
        }
        else if (static_cast<MSAA>(i) == MSAA::SAMPLE8)
        {
            amountSamples = 8;
        }
        TextureDesc colorTexDescMSAA = {
            .texFormat = TextureFormat::RGB, .texIntFormat = TextureInternalFormat::RGB_8, .mipLevels = 1,
            .uWrapping = TextureWrapping::ClampToEdge, .vWrapping = TextureWrapping::ClampToEdge,
            .width = m_width, .height = m_height, .magFilter = TextureFilter::Linear,
            .minFilter = TextureFilter::Linear,
            .texType = TextureType::Texture_2D_Multisample, .amountSamples = amountSamples
        };
        std::shared_ptr<Texture> colorTexMSAA4 = std::make_shared<Texture>(colorTexDescMSAA);
        RenderBufferDesc depthStencilRenderBufferDescMSAA = {
            .intFormat = RenderBufferInternalFormat::DEPTH24_STENCIL_8,
            .renderBufferType = RenderBufferType::Multisampled,
            .amountSamples = amountSamples,
            .width = m_width,
            .height = m_height

        };

        std::shared_ptr<RenderBuffer> depthStencilRenderBufferMSAA = std::make_shared<RenderBuffer>(
            depthStencilRenderBufferDescMSAA);

        std::vector<FrameBufferAttachment> colorAttachmentsMSAA = {
            {.attachment = colorTexMSAA4, .typeAttachment = FrameBufferTypeAttachment::Color}
        };
        std::vector<FrameBufferAttachment> depthStencAttachmentsMSAA = {
            {.attachment = depthStencilRenderBufferMSAA, .typeAttachment = FrameBufferTypeAttachment::DepthStencil}
        };
        m_msaaFB[i] = std::make_unique<FrameBuffer>(colorAttachmentsMSAA, depthStencAttachmentsMSAA, m_width, m_height);
        m_msaaFB[i]->SetName("Msaa framebuffer " + std::to_string(amountSamples));
    }

    //Initialize shadow debug frameBuffer
    TextureDesc texDescColorShadowDebug = {
        .texFormat = TextureFormat::RGB, .texIntFormat = TextureInternalFormat::SRGB8, .mipLevels = 1,
        .uWrapping = TextureWrapping::ClampToEdge, .vWrapping = TextureWrapping::ClampToEdge,
        .width = 1024, .height = 1024, .magFilter = TextureFilter::Linear, .minFilter = TextureFilter::Linear
    };
    auto texColorShadowDebug = std::make_shared<Texture>(texDescColorShadowDebug);


    std::vector<FrameBufferAttachment> colorShadowDebugAttachments = {
        {.attachment = texColorShadowDebug, .typeAttachment = FrameBufferTypeAttachment::Color}
    };

    m_shadowFrameBufferDebug = std::make_unique<FrameBuffer>(colorShadowDebugAttachments, emptyDepthStencAttach, 1024,
                                                             1024);
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


    //Sort visible entities for cam
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

    //Sort visible entities for light

    for (auto sceneEntity : m_visibleLightEntities)
    {
        auto mesh = sceneEntity->GetModelRenderInfo()->GetMesh();

        if (mesh->GetUsingInstancing())
        {
            const auto& submeshes = mesh->GetSubMeshes();
            for (unsigned int i = 0; i < submeshes.size(); i++)
            {
                DrawKeyInstancingShadow drawKey = {
                    .mesh = mesh, .submeshIndex = i,

                };

                if (!m_drawKeyToShadowInstanceItem.contains(drawKey))
                {
                    m_drawKeyToShadowInstanceItem[drawKey] = {.mesh = mesh, .transforms = {}};
                }

                m_drawKeyToShadowInstanceItem.at(drawKey).transforms.push_back(sceneEntity->GetWorldMat());
            }
        }
        else
        {
            m_shadowPassEntities.push_back(sceneEntity);
        }
    }


    // Sort entities for frustum pass
    if (m_renderFrustums)
    {
        for (auto& camEntity : m_currScene->GetCameras())
        {
            if (camEntity->GetComponent<Camera>()->GetViewFrustum())
            {
                m_frustumPass.push_back(camEntity);
            }
        }
    }
}

void Renderer::RenderScene()
{
    ZoneScoped;

    //Clean default FB
    glBindBuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


    BindFrameBuffer();

    auto resolvedFB = ResolveFrameBuffer();
    resolvedFB->CleanFrameBuffer(BlitMode::COLOR | BlitMode::DEPTH | BlitMode::STENCIL, {}, 0);
    m_shadowFrameBuffer->CleanFrameBuffer(BlitMode::DEPTH);


    UploadDataPerFrameUBOSAndSSBO();

    if (!m_currScene)
    {
        std::cout << "There is no scene assigned to the renderer" << std::endl;
        return;
    }

    FrustumCulling();
    SortRenderItems();
    m_shadowViewPort->BindViewPort();
    ShadowPass();
    InstancingShadowPass();
    BindFrameBuffer();
    m_sceneViewport->BindViewPort();


    BBPass();
    FrustumPass();

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

    ResolveMSAAFrameBuffer();
    ShadowPassDebug();
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

void Renderer::ResolveMSAAFrameBuffer()
{
    if (!m_usingMSAA)
    {
        return;
    }
    auto currMsaaFB = m_msaaFB[static_cast<int>(m_msaaMode)].get();

    currMsaaFB->BlitFrameBuffer(m_forwardFrameBuffer.get(), BlitMode::COLOR, BlitFilter::Linear, 0, 0, m_width,
                                m_height,
                                0, 0, m_width, m_height);
}

void Renderer::BindFrameBuffer()
{
    ResolveFrameBuffer()->BindFrameBuffer();
}

FrameBuffer* Renderer::ResolveFrameBuffer()
{
    if (!m_usingMSAA)
    {
        return m_forwardFrameBuffer.get();
    }
    return m_msaaFB[static_cast<int>(m_msaaMode)].get();
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
    shader->setUniformPerName(ShaderBasicProperties::UsingBlin, m_usingBlin);


    // Upload material dynamic

    const auto& dynamicPropertiesMaterial = material->GetShaderUniformValues();
    this->UploadUniformProperties(dynamicPropertiesMaterial, shader);
}

void Renderer::UploadPerFrameProperties(Shader* shader)
{
    const auto& activeProperties = shader->GetActiveProperties();
    const auto& lights = m_currScene->GetLights();
    auto activeCamera = m_currScene->GetActiveCamera()->GetComponent<Camera>();


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
        else if (shaderProperty == ShaderBasicProperties::EyePosition && activeCamera)
        {
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

        else if (shaderProperty == ShaderBasicProperties::NearPlane)
        {
            auto nearPlane = UniformValue{activeCamera->GetNearPlane()};
            shader->setUniformPerName(Shader::ShaderPropertyEnumToStr(shaderProperty), nearPlane);
        }
        else if (shaderProperty == ShaderBasicProperties::FarPlane)
        {
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

void Renderer::UploadShadowProperties(Shader* shader)
{
    auto shadowMap = m_shadowFrameBuffer->GetDepthAttachmentTexture();
    if (shadowMap)
    {
        shadowMap->SetTextureSlot(SHADOW_MAP_SLOT);
        shadowMap->BindTexture();
        shader->setUniformPerName(ShaderBasicProperties::ShadowMap, shadowMap);
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
    const auto& activeCamComponent = activeCam->GetComponent<Camera>();
    activeCamComponent->CalculateFrustum(static_cast<float>(m_width) / static_cast<float>(m_height));

    Light* lightWithShadowsDir = nullptr;
    for (const auto& lightEntity : m_currScene->GetLights())
    {
        auto lightComponent = lightEntity->GetComponent<Light>();

        if (!lightComponent)
        {
            continue;
        }
        if (lightComponent->GetLightType() == LightType::DirectionalLight && lightComponent->GetCastShadows())
        {
            lightWithShadowsDir = lightComponent;
            break;
        }
    }


    for (const auto& sceneEntity : m_currScene->GetEntities())
    {
        auto renderInfo = sceneEntity->GetModelRenderInfo();


        if (!renderInfo)
        {
            continue;
        }
        if (!renderInfo->IsEnabled())
        {
            continue;
        }
        m_totalRenderableEntities += 1;


        auto worldBoundingVol = renderInfo->GetWorldAABB();
        if (!worldBoundingVol)
        {
            continue;
        }

        if (activeCamComponent->IsInViewFrustum(worldBoundingVol))
        {
            m_visibleEntities.push_back(sceneEntity.get());
            if (m_renderBB)
            {
                m_worldBBVolumes.push_back(sceneEntity.get());
            }
        }

        if (!lightWithShadowsDir)
        {
            continue;
        }

        lightWithShadowsDir->GetViewMat();
        lightWithShadowsDir->GetOrthoProjMatFromLightFrustum(
            activeCamComponent->GetProjMatrix(
                static_cast<float>(m_width) / static_cast<float>(m_height)) * activeCamComponent->GetViewMatrix());
        lightWithShadowsDir->CalculateFrustum();


        if (lightWithShadowsDir->IsInViewFrustum(worldBoundingVol))
        {
            m_visibleLightEntities.push_back(sceneEntity.get());
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


    return camFrustum;
}


void Renderer::ShadowPassDebug()
{
    if (!m_debugPassShadowMap)
    {
        return;
    }
    m_shadowDebugViewPort->BindViewPort();
    m_shadowFrameBufferDebug->BindFrameBuffer();

    m_shadowFrameBufferDebug->CleanFrameBuffer(BlitMode::COLOR, {.color = glm::vec4(1.0f)}, 0);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);

    if (m_gammaCorrection)
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }

    auto shadowPassDebugShader = ShaderManager::GetDefaultShader(DefaultShader::ShadowPassDebug);
    shadowPassDebugShader->Use();
    UploadShadowProperties(shadowPassDebugShader.get());

    glBindVertexArray(m_fullScreenQuad->GetVao());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glEnable(GL_DEPTH_TEST);
    if (m_gammaCorrection)
    {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }
    SetFaceCulling(m_faceCulling);
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
        UploadShadowProperties(objectShader);


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
    // m_width = width;
    // m_height = height;
    // glViewport(0, 0, width, height);
    //
    //
    // m_forwardFrameBuffer->ResizeColorAttachment(0, width, height);
    // m_forwardFrameBuffer->ResizeDepthAttachment(width, height);
    //
    // for (int i = 0; i < m_msaaFB.size(); i++)
    // {
    //     auto currMSAAFb = m_msaaFB[i].get();
    //     currMSAAFb->ResizeColorAttachment(0, width, height);
    //     currMSAAFb->ResizeDepthAttachment(width, height);
    // }
}

void Renderer::SetSceneViewPortResolution(unsigned int width, unsigned height)
{
    m_width = width;
    m_height = height;

    m_forwardFrameBuffer->ResizeAttachments(width, height);


    for (int i = 0; i < m_msaaFB.size(); i++)
    {
        auto currMSAAFb = m_msaaFB[i].get();
        currMSAAFb->ResizeAttachments(width, height);
    }

    m_postProcessFrameBuffer->ResizeAttachments(width, height);
}


#define X(name, val) #name

const std::vector<std::string>& Renderer::GetFaceCullingNames()
{
    static std::vector<std::string> faceCullNames = {ENUM_FACE_CULLING(X)};
    return faceCullNames;
}

const std::vector<std::string>& Renderer::GetPostProcessEffectNames()
{
    static std::vector<std::string> postProcessNames = {ENUM_FACE_POST_PROCESS_EFFECT(X)};
    return postProcessNames;
}

const std::vector<std::string>& Renderer::GetMSAANames()
{
    static std::vector<std::string> msaaNames = {MSAA_MODE_ENUM(X)};
    return msaaNames;
}


#undef X

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

void Renderer::SetUsingMSAA(bool usingMsaa)
{
    m_usingMSAA = usingMsaa;
    if (m_usingMSAA)
    {
        glEnable(GL_MULTISAMPLE);
    }
    else
    {
        glDisable(GL_MULTISAMPLE);
    }
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
    UploadShadowProperties(shader);
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
    for (auto& [drawKey, shadowInstancingDrawItem] : m_drawKeyToShadowInstanceItem)
    {
        shadowInstancingDrawItem.transforms.clear();
    }
    m_opaqueCutOutEntities.clear();
    m_semiTransparentEntities.clear();
    m_borderPassEntities.clear();
    m_visibleEntities.clear();
    m_worldBBVolumes.clear();
    m_normalPassEntities.clear();
    m_shadowPassEntities.clear();
    m_visibleLightEntities.clear();
    m_frustumPass.clear();


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

void Renderer::FrustumPass()
{
    if (!m_renderFrustums)
    {
        return;
    }

    auto currCam = m_currScene->GetActiveCamera()->GetComponent<Camera>();
    auto frustumViewerShader = ShaderManager::GetDefaultShader(DefaultShader::FrustumViewer);
    frustumViewerShader->Use();

    for (auto& frustumEntity : m_frustumPass)
    {

        auto frustumCamComponent = frustumEntity->GetComponent<Camera>();
        frustumViewerShader->setUniformPerName(ShaderBasicProperties::ViewTransform,
                                               frustumCamComponent->GetViewFrustum());
        frustumViewerShader->setUniformPerName(ShaderBasicProperties::ProjectionTransform,
                                               frustumCamComponent->GetProjMatrix(
                                                   static_cast<float>(m_width) / static_cast<float>(m_height)));

        glDrawElements(GL_POINTS, 0, GL_UNSIGNED_INT, nullptr);
    }
    // auto currDirLight = m_currScene->GetCurrDirLight();
    // if (!currDirLight)
    // {
    //     return;
    // }
    // auto currDirLightComponent = currDirLight->GetComponent<Light>();
    // if (!currDirLightComponent)
    // {
    //     return;
    // }
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

unsigned int Renderer::GetPostProccesEffectColTex() const
{
    return m_postProcessFrameBuffer->GetColorAttachmentTexture(0)->GetTextureId();
}

void Renderer::ShadowPass()
{
    m_shadowFrameBuffer->BindFrameBuffer();
    auto shadowPassShader = ShaderManager::GetDefaultShader(DefaultShader::ShadowPass);
    shadowPassShader->Use();


    for (auto entity : m_shadowPassEntities)
    {
        auto mesh = entity->GetModelRenderInfo()->GetMesh();
        glBindVertexArray(mesh->GetVao());

        shadowPassShader->setUniformPerName(ShaderBasicProperties::ModelTransform, entity->GetWorldMat());

        for (auto submesh : mesh->GetSubMeshes())
        {
            auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
            glDrawElements(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset);
        }
    }
}

void Renderer::InstancingShadowPass()
{
    for (const auto& [drawKeyShadow, instanceShadowItem] : m_drawKeyToShadowInstanceItem)
    {
        if (instanceShadowItem.transforms.size() == 0)
        {
            continue;
        }
        auto mesh = instanceShadowItem.mesh;
        auto instancingLayout = mesh->GetVertexLayout();
        int transformAttrLoc = instancingLayout->GetVertexAttribLocation(VertexPredefinedAttributes::TRANSFORM);


        auto shadowPassInstancingShader = ShaderManager::GetDefaultShader(
            DefaultShader::ShadowPassInstancing, transformAttrLoc);


        if (!shadowPassInstancingShader)
        {
            std::cerr << "Was not found an apropiated shader for the shadow pass instancing for the mesh " << mesh->
                GetMeshKey() << std::endl;
            continue;
        }
        shadowPassInstancingShader->Use();

        auto sizeTransforms = instanceShadowItem.transforms.size() * sizeof(glm::mat4);
        mesh->SetInstancesTransform(instanceShadowItem.transforms.data(), sizeTransforms);
        glBindVertexArray(mesh->GetVao());
        for (const auto& submesh : mesh->GetSubMeshes())
        {
            auto indexOffset = (void*)(uintptr_t)(submesh.indexOffset * sizeof(uint32_t));
            glDrawElementsInstanced(GL_TRIANGLES, submesh.indexCount, GL_UNSIGNED_INT, indexOffset,
                                    instanceShadowItem.transforms.size());
        }
    }
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

    if (m_gammaCorrection)
    {
        glEnable(GL_FRAMEBUFFER_SRGB);
    }
    m_sceneViewport->BindViewPort();
    m_postProcessFrameBuffer->BindFrameBuffer();


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
    if (m_gammaCorrection)
    {
        glDisable(GL_FRAMEBUFFER_SRGB);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    SetFaceCulling(m_faceCulling);

    glBlitNamedFramebuffer(m_postProcessFrameBuffer->GetFrameBufferId(), 0, 0, 0, m_width, m_height, 0, 0, m_width,
                           m_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
}

void Renderer::UploadDataPerFrameUBOSAndSSBO()
{
    auto currCam = m_currScene->GetActiveCamera()->GetComponent<Camera>();

    CamMatrices camMatrix = {
        .viewMatrix = currCam->GetViewMatrix(),
        .projMatrix = currCam->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height)),
    };
    bool needsUBOReallocate;
    m_UBOCamMatrix->BufferUploadData(&camMatrix, 0, sizeof(CamMatrices), needsUBOReallocate);


    //Upload lights to UBO


    auto& sceneLights = m_currScene->GetLights();

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
    bool needsSSBOReallocate;
    m_SSBOLights->BufferUploadData(m_lightsGpu.data(), 0, sceneLights.size() * sizeof(LightGPU),
                                   needsSSBOReallocate);

    if (auto currDirLight = m_currScene->GetCurrDirLight()->GetComponent<Light>())
    {
        CamMatrices lightMatrices = {
            .viewMatrix = currDirLight->GetViewMat(),
            .projMatrix = currDirLight->GetOrthoProjMatFromLightFrustum(
                currCam->GetProjMatrix(static_cast<float>(m_width) / static_cast<float>(m_height)) * currCam->
                GetViewMatrix()),
        };
        m_UBOLightMatrix->BufferUploadData(&lightMatrices, 0, sizeof(camMatrix), needsUBOReallocate);
        //m_UBOCamMatrix->BufferUploadData(&lightMatrices, 0, sizeof(camMatrix), needsUBOReallocate);
    }
}
