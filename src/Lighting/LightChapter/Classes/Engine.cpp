#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "glad/glad.h"
#include "Engine.h"


#include <iostream>

#include <format>
#include <fstream>
#include <iostream>
#include "Shader.h"
#include "Texture.h"
#include <vector>


#include "AssimpLoader.h"
#include "BasicShapesMeshGenerator.h"
#include "Material.h"
#include "VertexLayout.h"
#include "Mesh.h"
#include "../Components/ModelRenderInfo.h"
#include "Renderer.h"
#include "Scene.h"
#include "MouseHandler.h"
#include "RandomCoordinateGenerator.h"
#include "glad/glad.h"
#include "../Components/Camera.h"
#include "Window.h"
#include "Scene.h"

#include "../Components/Light.h"
#include "ShaderManager.h"
#include "imgui.h"
#include "ShaderPermutatorGenerator.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"
#include "tracy/Tracy.hpp"


double Engine::m_lastFrameTime;
double Engine::m_deltaTime;
double Engine::m_currFrameTime;

Engine* Engine::engine = nullptr;


void Engine::Initialize()
{
    Engine::engine = this;
    m_window = std::make_unique<Window>();
    m_window->Initialize(this);
    const char* vendor = (const char*)glGetString(GL_VENDOR);
    const char* renderer = (const char*)glGetString(GL_RENDERER);
    const char* version = (const char*)glGetString(GL_VERSION);

    std::cout << "GL_VENDOR   : " << (vendor ? vendor : "null") << "\n";
    std::cout << "GL_RENDERER : " << (renderer ? renderer : "null") << "\n";
    std::cout << "GL_VERSION  : " << (version ? version : "null") << "\n";


    // glEnable(GL_DEBUG_OUTPUT);
    // glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    // glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
    //                           GLsizei length, const GLchar* message, const void* userParam)
    // {
    //     // std::cerr << "GL DEBUG: " << message << "\n";
    //     auto const src_str = [source]()
    //     {
    //         switch (source)
    //         {
    //         case GL_DEBUG_SOURCE_API: return "API";
    //         case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
    //         case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
    //         case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
    //         case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
    //         case GL_DEBUG_SOURCE_OTHER: return "OTHER";
    //         }
    //     }();
    //
    //     auto const type_str = [type]()
    //     {
    //         switch (type)
    //         {
    //         case GL_DEBUG_TYPE_ERROR: return "ERROR";
    //         case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
    //         case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
    //         case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
    //         case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
    //         case GL_DEBUG_TYPE_MARKER: return "MARKER";
    //         case GL_DEBUG_TYPE_OTHER: return "OTHER";
    //         }
    //     }();
    //
    //     auto const severity_str = [severity]()
    //     {
    //         switch (severity)
    //         {
    //         case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
    //         case GL_DEBUG_SEVERITY_LOW: return "LOW";
    //         case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
    //         case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
    //         }
    //     }();
    //     std::cerr << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n' <<
    //         std::endl;
    // }, nullptr);

    glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());


    glEnable(GL_DEPTH_TEST);

    m_idOnChangeWindowResolution = m_window->m_OnViewPortSizeChange.AddListener(std::bind(
        &Engine::OnChangeWindowResolution, this, std::placeholders::_1,
        std::placeholders::_2));

    m_idOnMouseMovement = m_window->m_OnMouseMovement.AddListener([this](auto&& PH1, auto&& PH2)
    {
        OnMouseMovement(std::forward<decltype(PH1)>(PH1), std::forward<decltype(PH2)>(PH2));
    });
    m_window->m_OnMouseScroll.AddListener(std::bind(&Engine::OnMouseScroll, this, std::placeholders::_1,
                                                    std::placeholders::_2));

    m_lastFrameTime = m_window->GetTime();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplGlfw_InitForOpenGL(m_window->GetGLFWWindow(), true);
    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();

    m_mouseHandler = std::make_unique<MouseHandler>(m_window->GetWidth(), m_window->GetHeight());
    m_renderer = std::make_unique<Renderer>(m_window->GetWidth(), m_window->GetHeight());
    InitializeDefaultShaders();
}

void Engine::InitializeDefaultShaders()
{
    std::shared_ptr<Shader> borderShader = std::make_shared<Shader>("Shaders/borderShader.vert",
                                                                    "Shaders/borderShader.frag");
    ShaderManager::AddDefaultShader(borderShader, DefaultShader::BorderColor);

    std::shared_ptr<Shader> aabbShader = std::make_unique<Shader>("Shaders/Debug/bbViewer.vert",
                                                                  "Shaders/Debug/bbViewer.frag",
                                                                  "Shaders/Debug/geoBBViewer.glsl");

    ShaderManager::AddDefaultShader(aabbShader, DefaultShader::AABB);


    for (int i = 0; i < 5; i++)
    {
        ShaderPermutatorGenerator::CreateNormalViewerShader(i);
    }

    std::shared_ptr<Shader> fullScreenQUad = std::make_shared<Shader>("Shaders/fullScreenQuadVert.glsl",
                                                                      "Shaders/fullScreenQuadFrag.glsl");
    ShaderManager::AddDefaultShader(fullScreenQUad, DefaultShader::FullScreenQuad);

    std::shared_ptr<Shader> skyboxShader = std::make_shared<Shader>("Shaders/SkyBoxVert.glsl",
                                                                    "Shaders/SkyBoxFrag.glsl");

    ShaderManager::AddDefaultShader(skyboxShader, DefaultShader::Skybox);

    auto shadowPassShader = std::make_shared<Shader>("Shaders/shadowPassVert.glsl",
                                                     "Shaders/shadowPassFrag.glsl");
    ShaderManager::AddDefaultShader(shadowPassShader, DefaultShader::ShadowPass);

    for (int i = 3; i < 10; i++)
    {
        ShaderPermutatorGenerator::CreateShadowInstancingShader(i);
    }

    auto shadowPassDebugShader = std::make_shared<Shader>("Shaders/fullScreenQuadVert.glsl",
                                                          "Shaders/Debug/shadowPassDebugFrag.glsl");
    ShaderManager::AddDefaultShader(shadowPassDebugShader, DefaultShader::ShadowPassDebug);

    auto frustumViewer = std::make_shared<Shader>("Shaders/Debug/frustumViewerVert.glsl",
                                                  "Shaders/Debug/frustumViewerFrag.glsl",
                                                  "Shaders/Debug/frustumViewerGeo.glsl");
    ShaderManager::AddDefaultShader(frustumViewer, DefaultShader::FrustumViewer);
}


void Engine::SetScene()
{
    std::unique_ptr<Scene> scene = std::make_unique<Scene>();


    //Normal cube
    std::vector<VertexAttribute> verticesLayout = {
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::POSITION),
            .amountComponents = 3
        },
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::COLOR),
            .amountComponents = 4
        },
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::UV),
            .amountComponents = 2
        },
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::NORMAL),
            .amountComponents = 3
        },
    };


    std::shared_ptr<VertexLayout> verticesShaderLayout = std::make_shared<VertexLayout>(verticesLayout);


    std::shared_ptr<Shader> shaderLight = std::make_shared<Shader>("Shaders/vertexShader.vert",
                                                                   "Shaders/fragmentShader.vert");

    ShaderManager::AddShader(shaderLight);

    std::shared_ptr<Material> cubeMaterial = std::make_unique<Material>("WoodHappy");
    cubeMaterial->SetShader(shaderLight.get());


    auto cubeModel = BasicShapesMeshGenerator::CreateCubeMesh(cubeMaterial, verticesShaderLayout);

    shaderLight->setPrintDebug(true);

    std::unique_ptr<ModelRenderInfo> cube = std::make_unique<ModelRenderInfo>(cubeModel);

    //Cube happy entity
    std::unique_ptr<Entity> cubeEntity = std::make_unique<Entity>();
    cubeEntity->SetModelRenderInfo(std::move(cube));
    auto cubePos = cubeEntity->GetLocalPos();
    cubePos.z = -5.0f;
    cubeEntity->SetLocalPos(cubePos);

    cubeEntity->SetUpdate([](Entity* entity, float deltaTime)
    {
        auto entityPos = entity->GetLocalPos();
        entityPos.y = sin(Engine::GetTime());
        entity->SetLocalPos(entityPos);
    });
    cubeEntity->SetName("CubeContainerEntity");
    std::unique_ptr<OutlineComponent> outlineComponent = std::make_unique<OutlineComponent>();
    cubeEntity->AddComponent(std::move(outlineComponent));
    scene->AddEntity(std::move(cubeEntity));

    //LightCube
    std::vector<VertexAttribute> verticesLayoutDebugLightShader = {
        {.type = VertexAttributeType::FLOAT, .attributeName = "vertexPos", .amountComponents = 3},
    };
    std::shared_ptr<Shader> lightDebugShader = std::make_shared<Shader>("Shaders/lightDebug.vert",
                                                                        "Shaders/lightDebug.frag");
    ShaderManager::AddShader(lightDebugShader);


    std::shared_ptr<Material> lightCubeMaterial = std::make_shared<Material>("White material");
    auto cubeModelLight = BasicShapesMeshGenerator::CreateCubeMesh(lightCubeMaterial, verticesShaderLayout);
    lightCubeMaterial->SetShader(lightDebugShader.get());
    std::unique_ptr<ModelRenderInfo> cubeLight = std::make_unique<ModelRenderInfo>(cubeModelLight);
    std::unique_ptr<Light> spotLight = std::make_unique<Light>();
    spotLight->m_attenuationLinear = 0.09f;
    spotLight->m_attenuationQuadratic = 0.032f;
    std::unique_ptr<Entity> pointLightEntity = std::make_unique<Entity>();
    pointLightEntity->SetLocalPos(glm::vec3(0.0, 1.0f, -4.5f));
    pointLightEntity->SetModelRenderInfo(std::move(cubeLight));
    pointLightEntity->AddComponent(std::move(spotLight));
    pointLightEntity->SetUpdate([](Entity* entity, float deltaTime)
    {
        float r = 3.5f;
        float posY = r * sin(Engine::GetTime());
        float posZ = r * cos(Engine::GetTime()) - 5.0f;

        auto entityPos = entity->GetLocalPos();
        entityPos.y = posY;
        entityPos.z = posZ;
        entity->SetLocalPos(entityPos);
    });
    pointLightEntity->SetName("pointLightEntity");


    std::shared_ptr<Texture> woodenContainerDiffuse = std::make_shared<Texture>();
    woodenContainerDiffuse->LoadImageFromFile("textures/container2Diffuse.png", true, true);


    std::shared_ptr<Texture> woodenContainerSpecular = std::make_shared<Texture>();;
    woodenContainerSpecular->LoadImageFromFile("textures/container2Specular.png", true, false);

    std::shared_ptr<Texture> happyFace = std::make_shared<Texture>();
    happyFace->LoadImageFromFile("textures/awesomeface.png", true, true);


    cubeMaterial->SetUniformValue("happyFace", UniformValue{happyFace});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Diffuse, UniformValue{woodenContainerDiffuse});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Specular, UniformValue{woodenContainerSpecular});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Shininess, 32);


    // Grass entity
    auto grassCutTexture = std::make_shared<Texture>();
    grassCutTexture->LoadImageFromFile("textures/grass.png");

    auto grassShader = std::make_shared<Shader>("Shaders/vertexShader.vert", "Shaders/grassShaderFrag.glsl");
    ShaderManager::AddShader(grassShader);
    auto grassMaterial = std::make_shared<Material>("GrassMaterial");
    grassMaterial->SetProperty(MaterialPropertyEnum::Diffuse, grassCutTexture);
    grassMaterial->SetShader(grassShader.get());
    auto quadGrass = BasicShapesMeshGenerator::CreateQuad(grassMaterial, verticesShaderLayout);
    auto grassEntity = std::make_unique<Entity>();
    grassEntity->SetLocalPos(glm::vec3(7.0f, 0.0f, -5.0f));
    auto grassRenderInfo = std::make_unique<ModelRenderInfo>(quadGrass);
    grassEntity->SetName("Grass entity");
    grassEntity->SetModelRenderInfo(std::move(grassRenderInfo));
    grassMaterial->SetTransparencyType(TransparencyType::Transparent);
    scene->AddEntity(std::move(grassEntity));


    //Lights entity
    auto lightPosX = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(-10, 10);
    auto lightPosy = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(-20, 20);
    auto lightPosz = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(-10, 10);


    int amountLights = 0;

    for (int i = 0; i < amountLights; i++)
    {
        auto lightEntity = std::make_unique<Entity>();
        auto renderInfo = std::make_unique<ModelRenderInfo>(cubeModel);

        lightEntity->SetModelRenderInfo(std::move(renderInfo));
        auto lightComponent = std::make_unique<Light>();
        lightComponent->m_lightType = LightType::PointLight;
        lightEntity->AddComponent(std::move(lightComponent));
        lightEntity->SetLocalPos(glm::vec3(lightPosX->GetRandom(), lightPosy->GetRandom(),
                                           lightPosz->GetRandom()));

        scene->AddEntity(std::move(lightEntity));
    }


    MouseHandler mouseHandler = MouseHandler(m_window->GetWidth() / 2.0f, m_window->GetHeight() / 2.0f);

    std::unique_ptr<Camera> camera = std::make_unique<Camera>();


    camera->SetMouseHandler(&mouseHandler);


    auto spotLighComponent = std::make_unique<Light>();
    spotLighComponent->m_lightType = LightType::SpotLight;
    spotLighComponent->SetInnerConeAngle(10.0f);
    spotLighComponent->SetOuterConeAngle(15.0f);
    auto spotLightEntity = std::make_unique<Entity>();
    spotLightEntity->AddComponent(std::move(spotLighComponent));
    spotLightEntity->SetUpdate([](Entity* entity, float deltaTime)
        {
            auto& activeCamera = entity->GetScene()->GetCameras()[0];
            auto camOrientation = activeCamera->GetLocalRot();
            camOrientation.y -= 180;
            camOrientation.x = -camOrientation.x;
            entity->SetLocalPos(activeCamera->GetLocalPos());
            entity->SetLocalRot(camOrientation);
        }
    );
    spotLightEntity->SetName("SpotLightEntity");

    auto dirLightComponent = std::make_unique<Light>();
    dirLightComponent->m_direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    dirLightComponent->m_lightType = LightType::DirectionalLight;
    dirLightComponent->SetCastShadows(true);

    auto dirLightEntity = std::make_unique<Entity>();
    dirLightEntity->SetLocalRot(glm::vec3(90.0f, 0.0f, 0.0f));


    dirLightEntity->SetName("DirLightEntity");
    dirLightEntity->AddComponent(std::move(dirLightComponent));
    std::unique_ptr<ModelRenderInfo> renderInfoLightEntity(
        dynamic_cast<ModelRenderInfo*>(pointLightEntity->GetModelRenderInfo()->Clone().release()));
    dirLightEntity->SetModelRenderInfo(std::move(renderInfoLightEntity));

    auto cameraEntity = std::make_unique<Entity>();

    cameraEntity->AddComponent(std::move(camera));
    cameraEntity->SetName("MainCamera");

    //Add camera
    scene->AddEntity(std::move(cameraEntity));

    //Add lights
    scene->AddEntity(std::move(pointLightEntity));
    scene->AddEntity(std::move(spotLightEntity));
    scene->AddEntity(std::move(dirLightEntity));


    float lastTime = glfwGetTime();


    auto shaderBackPack = std::make_shared<Shader>("Shaders/vertexShader.vert",
                                                   "Shaders/fragShaderBackpack.frag");
    ShaderManager::AddShader(shaderBackPack);
    auto shaderBackPackInstancing = std::make_shared<Shader>("Shaders/vertexShaderInstancing.vert",
                                                             "Shaders/fragShaderBackpack.frag");
    ShaderManager::AddShader(shaderBackPackInstancing);
    //AssimpLoader
    auto pathModel = "assets/survivalBackPack/backpack.obj";
    auto assimpLoader = std::make_unique<AssimpLoader>();
    assimpLoader->SetLoadInfo(scene.get(), pathModel, verticesShaderLayout);
    auto backPackEntity = assimpLoader->ImportScene();
    assimpLoader->CleanLoader();

    auto shaderBackPackInstancingPtr = shaderBackPackInstancing.get();
    auto setShaderChildren = [shaderBackPackInstancingPtr](Entity* entity)
    {
        if (entity->GetModelRenderInfo())
        {
            entity->GetModelRenderInfo()->SetShader(shaderBackPackInstancingPtr);
            entity->GetModelRenderInfo()->GetMesh()->SetInstancing();
        }
    };


    backPackEntity->ApplyFunctionToChildren(setShaderChildren);

    backPackEntity->SetLocalScale(glm::vec3(0.1, 0.1f, 0.1f));

    auto r = 1000.0f;
    auto amountBackpacks = 2;

    auto xIncrement = 5.0f;
    for (int i = 0; i < amountBackpacks; i++)
    {
        std::vector<std::unique_ptr<Entity>> copiedBackpackChildren = {};
        auto backpackIEntity = backPackEntity->CopyEntity(copiedBackpackChildren);
        auto xPosition = i * xIncrement;
        backpackIEntity->SetLocalPos(glm::vec3(xPosition, 0.0f, 0.0f));

        for (auto& child : copiedBackpackChildren)
        {
            scene->AddEntity(std::move(child));
        }
    }


    auto redWindowTex = std::make_shared<Texture>();
    redWindowTex->LoadImageFromFile("textures/semiTransparentWindow.png");
    auto semiTransparentWindowMaterial = std::make_shared<Material>("SemitransparentWindow");
    semiTransparentWindowMaterial->SetProperty(MaterialPropertyEnum::Diffuse, redWindowTex);
    semiTransparentWindowMaterial->SetShader(grassShader.get());
    auto windowQuad = BasicShapesMeshGenerator::CreateQuad(semiTransparentWindowMaterial, verticesShaderLayout);
    semiTransparentWindowMaterial->SetTransparencyType(TransparencyType::Semitransparent);


    for (float z = 5.0f; z > 0; z -= 1.0)
    {
        auto redWindowEntity = std::make_unique<Entity>();
        redWindowEntity->SetName("WindowEntity" + std::to_string(z));
        auto renderInfo = std::make_unique<ModelRenderInfo>(windowQuad);
        redWindowEntity->SetLocalPos(glm::vec3(0.0f, 0.0f, z));
        redWindowEntity->SetModelRenderInfo(std::move(renderInfo));
        scene->AddEntity(std::move(redWindowEntity));
    }
    assimpLoader->SetLoadInfo(scene.get(), "assets/planetMars/planet.obj", verticesShaderLayout);
    auto planetEntity = assimpLoader->ImportScene();
    assimpLoader->CleanLoader();

    planetEntity->ApplyFunctionToChildren([shaderBackPack](Entity* entity)
    {
        if (entity->GetModelRenderInfo())
        {
            entity->GetModelRenderInfo()->SetShader(shaderBackPack.get());
        }
    });

    planetEntity->SetLocalPos(glm::vec3(0.0f, -35, 0.0f));
    assimpLoader->CleanLoader();


    assimpLoader->SetLoadInfo(scene.get(), "assets/Rocks/rock.obj", verticesShaderLayout);
    auto rockEntity = assimpLoader->ImportScene();

    rockEntity->ApplyFunctionToChildren(setShaderChildren);


    float currDegrees = 0;
    auto degreeIncrement = 1.5;

    auto totalRocks = 1000;

    auto rockRotGen = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(0, 360);

    auto rockScaleGen = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(1, 3);

    auto posY = std::make_unique<RandomCoordinateGenerator<
        std::mt19937, std::uniform_real_distribution<float>>>(-10, 10);

    float radius = 100;
    for (int i = 0; i < totalRocks; i++)
    {
        std::vector<std::unique_ptr<Entity>> childrenCopiedEntity = {};

        auto copyRockEntity = rockEntity->CopyEntity(childrenCopiedEntity);
        copyRockEntity->SetEntityParent(planetEntity);


        copyRockEntity->SetLocalScale(glm::vec3(rockScaleGen->GetRandom(), rockScaleGen->GetRandom(),
                                                rockScaleGen->GetRandom()));
        copyRockEntity->SetLocalRot(
            glm::vec3(rockRotGen->GetRandom(), rockRotGen->GetRandom(), rockRotGen->GetRandom()));

        auto posX = radius * cos(glm::radians(currDegrees));
        auto posZ = radius * sin(glm::radians(currDegrees));

        copyRockEntity->SetLocalPos(glm::vec3(posX, posY->GetRandom(), posZ));

        currDegrees += degreeIncrement;


        for (auto& copiedEntity : childrenCopiedEntity)
        {
            scene->AddEntity(std::move(copiedEntity));
        }
    }


    //Skybox
    std::string prefixCubeMapFilePath = "textures/Skyboxes/BlueSky/";
    std::array<std::string, 6> cubeMapFiles = {
        prefixCubeMapFilePath + "right.jpg", prefixCubeMapFilePath + "left.jpg", prefixCubeMapFilePath + "top.jpg",
        prefixCubeMapFilePath + "bottom.jpg", prefixCubeMapFilePath + "front.jpg", prefixCubeMapFilePath + "back.jpg"
    };
    TextureDesc cubeMapTexDesc = {
        .uWrapping = TextureWrapping::ClampToEdge, .vWrapping = TextureWrapping::ClampToEdge,
        .magFilter = TextureFilter::Linear,
        .minFilter = TextureFilter::Linear
    };
    std::shared_ptr<Texture> cubeMapTexture = std::make_shared<Texture>();

    cubeMapTexture->LoadCubeMapFromFile(cubeMapFiles, cubeMapTexDesc, true);

    auto skyboxCube = BasicShapesMeshGenerator::CreateSkyboxCube();

    auto skybox = std::make_unique<Skybox>(cubeMapTexture, skyboxCube);

    scene->SetSkyBox(std::move(skybox));

    // Floor wood

    auto woodTexture = std::make_shared<Texture>();
    woodTexture->LoadImageFromFile("textures/wood.png");
    auto woodMaterial = std::make_shared<Material>("woodMaterial");
    woodMaterial->SetShader(shaderBackPack.get());
    woodMaterial->SetProperty(MaterialPropertyEnum::Diffuse, woodTexture);
    auto floorQuad = BasicShapesMeshGenerator::CreateQuad(woodMaterial, verticesShaderLayout);
    auto floorRenderInfo = std::make_unique<ModelRenderInfo>(floorQuad);

    auto floorEntity = std::make_unique<Entity>();
    floorEntity->SetModelRenderInfo(std::move(floorRenderInfo));
    floorEntity->SetLocalRot(glm::vec3(-90.0f, 0.0f, 0.0f));
    floorEntity->SetLocalPos(glm::vec3(0.0f, -3.0f, 0.0f));
    floorEntity->SetLocalScale(glm::vec3(10.0f));

    scene->AddEntity(std::move(floorEntity));

    m_renderer->SetScene(scene.get());

    auto secondCamEntity = std::make_unique<Entity>();
    secondCamEntity->SetName("Second cameraaaaa :D");
    secondCamEntity->AddComponent(std::move(std::make_unique<Camera>()));
    scene->AddEntity(std::move(secondCamEntity));

    m_scene = std::move(scene);
}

void Engine::OnChangeWindowResolution(int widht, int height) const
{
    m_renderer->SetResolution(widht, height);
}

double Engine::GetTime()
{
    return Engine::engine->m_window->GetTime();
}


void Engine::GameLoop()
{
    m_renderer->SetScene(m_scene.get());

    m_editor = std::make_unique<Editor>(this);

    while (!m_window->WindowShouldClose())
    {
        m_window->PollEvents();
        m_window->ProcessInput();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();


        m_currFrameTime = m_window->GetTime();
        m_deltaTime = m_currFrameTime - m_lastFrameTime;
        m_lastFrameTime = m_currFrameTime;
        auto cameraActive = m_scene->GetActiveCamera()->GetComponent<Camera>();

        const auto& sceneEntities = m_scene->GetEntities();

        for (auto& entity : sceneEntities)
        {
            entity->Update(m_deltaTime);
        }

        if (m_processInput)
        {
            cameraActive->ProcessMovement(m_window->GetGLFWWindow(), m_deltaTime);
        }

        m_renderer->RenderScene();

        glEnable(GL_FRAMEBUFFER_SRGB);
        m_editor->Render();
        {
            ZoneScopedN("RenderGui")
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        glDisable(GL_FRAMEBUFFER_SRGB);
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        {
            ZoneScopedN("SwapBuffers");
            m_window->SwapBuffers();
        }


        FrameMark;
    }
}

void Engine::OnMouseMovement(double xpos, double ypos)
{
    if (m_processInput)
    {
        m_mouseHandler->NotifyMouseMovement(xpos, ypos);
    }
    else
    {
        m_mouseHandler->SetWasStatic(true);
    }
}

void Engine::OnMouseScroll(double xpos, double ypos) const
{
    if (m_processInput)
    {
        m_mouseHandler->NotifyMouseScroll(xpos, ypos);
    }
    else
    {
        m_mouseHandler->SetWasStatic(true);
    }
}


MouseHandler* Engine::GetMouseHandler()
{
    return Engine::engine->m_mouseHandler.get();
}

void Engine::CheckProcessSceneViewInput(bool cursorInSceneViewport)
{
    if (m_startedProcessingInput)
    {
        m_processInput = m_window->GetButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        m_window->DisableCursor(m_processInput);
    }
    else
    {
        m_processInput = cursorInSceneViewport && m_window->GetButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);
        m_startedProcessingInput = m_processInput;
        m_window->DisableCursor(m_processInput);
    }
}


void Engine::SetCameraSpeed(float speed)
{
    m_scene->GetActiveCamera()->GetComponent<Camera>()->SetMovementSpeed(speed);
}

float Engine::GetCameraSpeed() { return m_scene->GetActiveCamera()->GetComponent<Camera>()->GetSpeed(); }

bool Engine::ImGUiWantsCaptureMouse() const
{
    auto& io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool Engine::ImGUiWantsCaptureKeyboard() const
{
    auto& io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}
