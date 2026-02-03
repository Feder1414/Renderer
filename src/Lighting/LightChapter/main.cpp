#include <format>
#include <fstream>
#include <iostream>
#include "OpenGL.h"
#include "Shader.h"
#include "Texture.h"
#include <vector>

#include <math.h>
#include <random>


#include "Material.h"
#include "Transform.h"
#include "VertexLayout.h"
#include "Mesh.h"
#include "ModelRenderInfo.h"
#include "Renderer.h"
#include "Scene.h"
#include "MouseHandler.h"
#include "Camera.h"
#include "RandomCoordinateGenerator.h"


namespace WindowState
{
    int width = 800;
    int height = 600;
}

namespace Time
{
    double time = 0.0f;
}

void OnViewportSizeChanged(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
    WindowState::width = width;
    WindowState::height = height;
}

void OnErrorWindow(int error, const char* description)
{
    std::cout << std::format("Error al crear ventana glwf {}", description);
}

void DebugMatrix4(glm::mat4 mat4)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            std::cout << i << "," << j << " " << mat4[i][j] << std::endl;
        }
    }
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

std::shared_ptr<Mesh> CreateCubeMesh(std::shared_ptr<Material> material, std::shared_ptr<VertexLayout> vertexLayout)
{
    std::vector<float> cubeVertices = {

        // Cara trasera
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        // Cara delantera
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        //Cara izquierda
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        // Cara derecha
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        // Cara abajo
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        //Cara superior
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    };

    std::vector<unsigned int> cubeIndices = {
        0, 1, 2,
        3, 4, 5,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35


    };

    std::vector<SubMesh> submeshes = {
    };
    submeshes.push_back({
        .indexOffset = 0,
        .indexCount = static_cast<unsigned int>(cubeIndices.size()),

        .vertexOffset = 0,
        .vertexCount = static_cast<unsigned int>(cubeVertices.size())

    });
    std::vector<std::shared_ptr<Material>> submeshToMaterial = {material};

    std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>(cubeVertices, cubeIndices, vertexLayout, submeshes,
                                                            submeshToMaterial);

    return cubeMesh;
}


int main()
{
    glfwInit();
    glfwSetErrorCallback(OnErrorWindow);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


    GLFWwindow* window = glfwCreateWindow(WindowState::width, WindowState::height, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
    glViewport(0, 0, WindowState::width, WindowState::height);

    glfwSetFramebufferSizeCallback(window, OnViewportSizeChanged);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    //Setear la escena ----------------------------------------------------------------


    //Normal cube
    std::vector<VertexAttribute> verticesLayout = {
        {.type = VertexAttributeType::FLOAT, .attributeName = "vertexPos", .amountComponents = 3},
        {.type = VertexAttributeType::FLOAT, .attributeName = "vertexCol", .amountComponents = 4},
        {.type = VertexAttributeType::FLOAT, .attributeName = "uv", .amountComponents = 2},
        {.type = VertexAttributeType::FLOAT, .attributeName = "normal", .amountComponents = 3},
    };

    std::shared_ptr<VertexLayout> verticesShaderLayout = std::make_shared<VertexLayout>(verticesLayout);


    std::unique_ptr<Shader> shaderLight = std::make_unique<Shader>("Shaders/vertexShader.vert",
                                                                   "Shaders/fragmentShader.vert");
    std::shared_ptr<Material> cubeMaterial = std::make_unique<Material>();
    cubeMaterial->SetShader(shaderLight.get());


    auto cubeModel = CreateCubeMesh(cubeMaterial, verticesShaderLayout);
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
        entityPos.y = sin(Time::time);
        entity->SetLocalPos(entityPos);
    });

    //LightCube
    std::vector<VertexAttribute> verticesLayoutDebugLightShader = {
        {.type = VertexAttributeType::FLOAT, .attributeName = "vertexPos", .amountComponents = 3},
    };
    std::unique_ptr<Shader> lightDebugShader = std::make_unique<Shader>("Shaders/lightDebug.vert",
                                                                        "Shaders/lightDebug.frag");

    std::unique_ptr<Material> lightCubeMaterial = std::make_unique<Material>();
    lightCubeMaterial->SetShader(lightDebugShader.get());
    std::unique_ptr<ModelRenderInfo> cubeLight = std::make_unique<ModelRenderInfo>(cubeModel);
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
        float posY = r * sin(Time::time);
        float posZ = r * cos(Time::time) - 5.0f;

        auto entityPos = entity->GetLocalPos();
        entityPos.y = posY;
        entityPos.z = posZ;
        entity->SetLocalPos(entityPos);
    });


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
    renderer->SetResolution(WindowState::width, WindowState::height);
    renderer->DebugRenderLights(true);


    MouseHandler mouseHandler = MouseHandler(WindowState::width / 2.0f, WindowState::width / 2.0f);

    std::unique_ptr<Camera> camera = std::make_unique<Camera>();

    glfwSetWindowUserPointer(window, camera.get());
    glfwSetCursorPosCallback(window, Camera::CalculateForwardVector);
    glfwSetScrollCallback(window, Camera::UpdateFOV);

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
            auto activeCameraPos = activeCamera->GetTransform().m_position;
            entity->SetLocalPos(activeCamera->GetTransform().m_position);
            spotLightComponent->m_direction = activeCamera->GetForward();
        }
    );

    auto dirLightComponent = std::make_unique<Light>();
    dirLightComponent->m_direction = glm::vec3(-1.0f, -1.0f, -1.0f);
    auto dirLightEntity = std::make_unique<Entity>();
    dirLightEntity->AddComponent(std::move(dirLightComponent));


    scene->AddCamera(std::move(camera));
    scene->AddEntity(std::move(pointLightEntity));
    scene->AddEntity(std::move(spotLightEntity));
    scene->AddEntity(std::move(dirLightEntity));


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    float lastTime = glfwGetTime();

    Time::time = lastTime;


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currTime = glfwGetTime();
        Time::time = currTime;
        float deltaTime = currTime - lastTime;
        lastTime = currTime;
        auto cameraActive = scene->GetActiveCamera();

        const auto& sceneEntities = scene->GetEntities();

        for (auto& entity : sceneEntities)
        {
            entity->Update(deltaTime);
        }

        cameraActive->ProcessMovement(window, deltaTime);
        renderer->RenderScene();


        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
