#include <format>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include "Shader.h"
#include "Texture.h"
#include <vector>

#include <math.h>
#include <random>

#include "Camera.h"
#include "Transform.h"


void OnViewportSizeChanged(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
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


struct Buffers
{
    unsigned int VAO, VBO, EBO, amountVertex, amountIndex;
};

struct ModelRenderInfo
{
    Buffers buffers;
    Shader* shaderProgram;
    std::vector<Texture*> texture = std::vector<Texture*>();
    Transform transform = Transform();
    bool renderMany = false;
    std::vector<Transform> transforms = std::vector<Transform>();
};

struct GeneralRenderInfo
{
    glm::mat4 viewMatrix = glm::mat4(1.0f);
    glm::mat4 projMatrix = glm::mat4(1.0f);
};

struct Range
{
    float minValue;
    float maxValue;
};


Buffers CreateVertexBuffer(float modelVertices[], unsigned int indices[], unsigned int sizeVertices,
                           unsigned int sizeIndices, unsigned int vertexCount, unsigned int indexCount)
{
    unsigned int modelVao;
    glGenVertexArrays(1, &modelVao);
    glBindVertexArray(modelVao);


    unsigned int modelVBO;
    glGenBuffers(1, &modelVBO);
    glBindBuffer(GL_ARRAY_BUFFER, modelVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeVertices, modelVertices, GL_STATIC_DRAW);

    unsigned int triangleEBO;
    glGenBuffers(1, &triangleEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);

    const int stride = 9 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)((3 + 4) * sizeof(float)));
    glEnableVertexAttribArray(2);


    return Buffers{
        .VAO = modelVao,
        .VBO = modelVBO,
        .amountVertex = vertexCount,
        .amountIndex = indexCount,

    };
}

void GenerateRandomCoordinates(std::vector<glm::vec3>& coordinates, const Range& range,
                               unsigned int amountCoordinates)
{
    std::default_random_engine gen;
    std::uniform_real_distribution<float> distribution(range.minValue, range.maxValue);

    for (int i = 0; i < amountCoordinates; i++)
    {
        auto coordinate = glm::vec3(0.f);
        coordinate.x = distribution(gen);
        coordinate.y = distribution(gen);
        coordinate.z = distribution(gen);
        coordinates.push_back(coordinate);
    }
}

std::vector<Transform> GenerateRandomTransforms(const Range& posRange, const Range& rotRange, const Range& scaleRange,
                                                unsigned int totalTransforms)
{
    auto transforms = std::vector<Transform>();

    auto positions = std::vector<glm::vec3>();

    auto rotations = std::vector<glm::vec3>();

    auto scales = std::vector<glm::vec3>();

    GenerateRandomCoordinates(positions, posRange, totalTransforms);
    GenerateRandomCoordinates(rotations, rotRange, totalTransforms);
    GenerateRandomCoordinates(scales, scaleRange, totalTransforms);


    for (int i = 0; i < totalTransforms; i++)
    {
        auto transform = Transform();
        transform.m_position = positions[i];
        transform.m_rotation = rotations[i];
        transform.m_scale = scales[i];

        transforms.push_back(transform);
    }

    return transforms;
}


void DrawModel(ModelRenderInfo (&buffers)[1], GeneralRenderInfo generalRenderInfo, float time)
{
    const float timeOffset = 0.032;
    for (int i = 0; i < 1; i++)
    {
        auto modelRenderInfo = buffers[i];

        modelRenderInfo.shaderProgram->Use();


        for (int i = 0; i < modelRenderInfo.texture.size(); i++)
        {
            const auto texture = modelRenderInfo.texture[i];
            texture->BindTexture();
        }


        float opacity = sin(time + i * timeOffset) / 2 + 0.5f;

        if (modelRenderInfo.renderMany)
        {
            for (int i = 0; i < modelRenderInfo.transforms.size(); i++)
            {
                auto transform = modelRenderInfo.transforms[i];
                auto viewProjMat = generalRenderInfo.projMatrix * generalRenderInfo.viewMatrix * transform.
                    GetAllTransformMatrix();
                modelRenderInfo.shaderProgram->setVariableMatrix4("transformMatrix",
                                                                  viewProjMat);
                modelRenderInfo.shaderProgram->setVariableFloat("opacity", opacity);
                glBindVertexArray(modelRenderInfo.buffers.VAO);
                glDrawElements(GL_TRIANGLES, modelRenderInfo.buffers.amountIndex, GL_UNSIGNED_INT, 0);
            }
        }
        else
        {
            //modelRenderInfo.transform.SetRotation(glm::vec3(0.0f, 90 * opacity, 0.0f));
            modelRenderInfo.shaderProgram->setVariableFloat("opacity", opacity);
            DebugMatrix4(modelRenderInfo.transform.GetAllTransformMatrix());
            modelRenderInfo.shaderProgram->setVariableMatrix4("transformMatrix",
                                                              modelRenderInfo.transform.GetAllTransformMatrix());
            glBindVertexArray(modelRenderInfo.buffers.VAO);
            glDrawElements(GL_TRIANGLES, modelRenderInfo.buffers.amountIndex, GL_UNSIGNED_INT, 0);
        }
    }
}


unsigned int CreateShadersAndProgram(const char* vertexShaderSource, const char* fragmentShaderSource)
{
    int succes;
    char infoLog[512];


    unsigned int vertexShader;

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &succes);
    if (!succes)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &succes);
    if (!succes)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    }

    unsigned int shaderProgram = glCreateProgram();

    glGetProgramiv(shaderProgram, GL_COMPILE_STATUS, &succes);
    if (!succes)
    {
        glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
    }

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderProgram;
}


void CreateVertexLayout()
{
}


int main()
{
    glfwInit();
    glfwSetErrorCallback(OnErrorWindow);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int width = 800, height = 600;

    GLFWwindow* window = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
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
    glViewport(0, 0, width, height);

    glfwSetFramebufferSizeCallback(window, OnViewportSizeChanged);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    float firstTriangleVertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f
    };

    unsigned int firstIndices[] = {
        0, 1, 2,
    };
    float secondTriangleVertices[] = {
        0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.75f, 0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 1.0f
    };


    unsigned int secondIndices[] = {
        0, 1, 2,
    };

    float cubeVertices[] = {

        // Cara trasera
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        // Cara delantera
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

        //Cara izquierda
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,

        // Cara derecha
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,

        // Cara abajo
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,

        //Cara superior
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f

    };

    unsigned int cubeIndices[] = {
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


    // auto buffersFirstTriangle = CreateVertexBuffer(firstTriangleVertices, firstIndices,
    //                                                sizeof(firstTriangleVertices), sizeof(firstIndices), 3, 3);
    // auto buffersSecondTriangle = CreateVertexBuffer(secondTriangleVertices, secondIndices,
    //                                                 sizeof(secondTriangleVertices), sizeof(secondIndices), 3, 3);

    auto cubeBuffers = CreateVertexBuffer(cubeVertices, cubeIndices, sizeof(cubeVertices), sizeof(cubeIndices), 36, 36);

    std::default_random_engine gen;

    std::uniform_real_distribution<double> distribution(-15.0f, 15.0f);


    Shader shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.vert");


    TextureInfo textureWooden{&shader, "texture0"};
    Texture woodenContainer = Texture(textureWooden);

    TextureInfo textureHappyFace{&shader, "texture1"};
    Texture happyFace = Texture(textureHappyFace);


    woodenContainer.LoadImage("textures/container.jpg");
    happyFace.LoadImage("textures/awesomeface.png");

    ModelRenderInfo cubeRenderInfo = ModelRenderInfo{
        .buffers = cubeBuffers, .shaderProgram = &shader
    };

    MouseHandler mouseHandler = MouseHandler(width / 2.0f, height / 2.0f);

    Camera camera = Camera();

    camera.SetMouseHandler(&mouseHandler);

    glfwSetWindowUserPointer(window, &camera);
    glfwSetCursorPosCallback(window, Camera::CalculateForwardVector);
    glfwSetScrollCallback(window, Camera::UpdateFOV);


    auto transforms = GenerateRandomTransforms({0, 1}, {0, 180}, {0.1, 2}, 10);
    cubeRenderInfo.renderMany = true;
    cubeRenderInfo.transforms = transforms;


    cubeRenderInfo.texture.push_back(&woodenContainer);
    cubeRenderInfo.texture.push_back(&happyFace);

    auto generalRenderInfo = GeneralRenderInfo{
        .viewMatrix = camera.GetViewMatrix(),
        .projMatrix = glm::perspective(glm::radians(camera.GetFov()), (float)width / (float)height, 0.1f, 100.0f)


    };


    // ModelRenderInfo firstShaderBuffer = ModelRenderInfo{
    //     .buffers = buffersFirstTriangle, .shaderProgram = &shader,
    // };
    // ModelRenderInfo secondShaderBuffer = ModelRenderInfo{
    //     .buffers = buffersSecondTriangle, .shaderProgram = &shader,
    // };
    //
    // firstShaderBuffer.texture.push_back(&woodenContainer);
    // firstShaderBuffer.texture.push_back(&happyFace);
    //
    // secondShaderBuffer.texture.push_back(&woodenContainer);
    // secondShaderBuffer.texture.push_back(&happyFace);
    //
    //
    // ModelRenderInfo shaderBuffers[2] = {firstShaderBuffer, secondShaderBuffer};
    ModelRenderInfo modelsRenderInfo[1] = {cubeRenderInfo};


    //std::cout << std::format("VBO id {}, VAO id {}", buffers.VBO, buffers.VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float currTime = glfwGetTime();
        float deltaTime = currTime - lastTime;
        lastTime = currTime;
        camera.ProcessMovement(window, deltaTime);


        generalRenderInfo.projMatrix = glm::perspective(glm::radians(camera.GetFov()), (float)width / (float)height,
                                                        0.1f, 100.0f);
        generalRenderInfo.viewMatrix = camera.GetViewMatrix();

        DrawModel(modelsRenderInfo, generalRenderInfo, glfwGetTime());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
