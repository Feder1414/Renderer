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
#include "ModelRenderInfo.h"
#include "Renderer.h"
#include "Scene.h"
#include "MouseHandler.h"
#include "RandomCoordinateGenerator.h"
#include "glad/glad.h"
#include "Camera.h"
#include "Window.h"
#include "Scene.h"
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"


double Engine::m_lastFrameTime;
double Engine::m_deltaTime;
double Engine::m_currFrameTime;

Engine* Engine::engine = nullptr;


void Engine::Initialize()
{
    Engine::engine = this;
    m_window = std::make_unique<Window>();
    m_window->Initialize(this);


    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity,
                              GLsizei length, const GLchar* message, const void* userParam)
    {
        std::cerr << "GL DEBUG: " << message << "\n";
    }, nullptr);

    glViewport(0, 0, m_window->GetWidth(), m_window->GetHeight());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    m_idOnChangeWindowResolution = m_window->viewPortSizeChange.AddListener(std::bind(
        &Engine::OnChangeWindowResolution, this, std::placeholders::_1,
        std::placeholders::_2));

    m_lastFrameTime = m_window->GetTime();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    ImGui_ImplGlfw_InitForOpenGL(m_window->GetGLFWWindow(), true);
    // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
    ImGui_ImplOpenGL3_Init();
}

void Engine::SetScene()
{
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


    std::unique_ptr<Shader> shaderLight = std::make_unique<Shader>("Shaders/vertexShader.vert",
                                                                   "Shaders/fragmentShader.vert");
    std::shared_ptr<Material> cubeMaterial = std::make_unique<Material>();
    cubeMaterial->SetShader(shaderLight.get());


    auto cubeModel = BasicShapesMeshGenerator::CreateCubeMesh(cubeMaterial, verticesShaderLayout);
    shaderLight->setPrintDebug(true);
    std::unique_ptr<ModelRenderInfo> cube = std::make_unique<ModelRenderInfo>(cubeModel);

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

    //LightCube
    std::vector<VertexAttribute> verticesLayoutDebugLightShader = {
        {.type = VertexAttributeType::FLOAT, .attributeName = "vertexPos", .amountComponents = 3},
    };
    std::unique_ptr<Shader> lightDebugShader = std::make_unique<Shader>("Shaders/lightDebug.vert",
                                                                        "Shaders/lightDebug.frag");


    std::shared_ptr<Material> lightCubeMaterial = std::make_shared<Material>();
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


    Texture woodenContainerDiffuse = Texture();
    woodenContainerDiffuse.LoadImageFromFile("textures/container2Diffuse.png");

    Texture woodenContainerSpecular = Texture();
    woodenContainerSpecular.LoadImageFromFile("textures/container2Specular.png");

    Texture happyFace = Texture();
    happyFace.LoadImageFromFile("textures/awesomeface.png");


    cubeMaterial->SetUniformValue("happyFace", UniformValue{&happyFace});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Diffuse, UniformValue{&woodenContainerDiffuse});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Specular, UniformValue{&woodenContainerSpecular});
    cubeMaterial->SetProperty(MaterialPropertyEnum::Shininess, 32);


    std::unique_ptr<Scene> scene = std::make_unique<Scene>();
    std::unique_ptr<Renderer> renderer = std::make_unique<Renderer>();


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


    scene->AddEntity(std::move(cubeEntity));
    renderer->SetScene(scene.get());
    renderer->SetResolution(m_window->GetWidth(), m_window->GetHeight());
    renderer->DebugRenderLights(true);


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
            auto spotLightComponent = entity->GetComponent<Light>();
            entity->SetLocalPos(activeCamera->GetLocalPos());
            spotLightComponent->m_direction = activeCamera->GetForward();
        }
    );
    spotLightEntity->SetName("SpotLightEntity");

    auto dirLightComponent = std::make_unique<Light>();
    dirLightComponent->m_direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    auto dirLightEntity = std::make_unique<Entity>();
    dirLightEntity->SetName("DirLightEntity");
    dirLightEntity->AddComponent(std::move(dirLightComponent));

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


    auto shaderBackPack = std::make_unique<Shader>("Shaders/vertexShader.vert", "Shaders/fragShaderBackpack.frag");
    //AssimpLoader
    auto pathModel = "assets/survivalBackPack/backpack.obj";
    auto assimpLoader = std::make_unique<AssimpLoader>(scene.get(), pathModel, verticesShaderLayout);
    auto entity = assimpLoader->ImportScene();

    auto shaderBackPackPtr = shaderBackPack.get();
    auto setShaderChildren = [shaderBackPackPtr](Entity* entity)
    {
        if (entity->GetModelRenderInfo())
        {
            entity->GetModelRenderInfo()->SetShader(shaderBackPackPtr);
        }
    };

    entity->ApplyFunctionToChildren(setShaderChildren);
    entity->SetLocalScale(glm::vec3(0.1, 0.1f, 0.1f));
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

        cameraActive->ProcessMovement(m_window->GetGLFWWindow(), m_deltaTime);
        m_renderer->RenderScene();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        m_window->SwapBuffers();

        // (Your code calls glfwSwapBuffers() etc.)
    }
}
