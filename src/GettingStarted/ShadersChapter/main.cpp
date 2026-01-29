#include <format>
#include <fstream>
#include <iostream>
#include <glad/glad.h>
#include <glfw3.h>
#include "Shader.h"

#include <math.h>


void OnViewportSizeChanged(GLFWwindow* window, const int width, const int height)
{
    glViewport(0, 0, width, height);
}

void OnErrorWindow(int error, const char* description)
{
    std::cout << std::format("Error al crear ventana glwf {}", description);
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
    unsigned int VAO, VBO, EBO;
};

struct ModelRenderInfo
{
    Buffers buffers;
    Shader* shaderProgram;
};


Buffers CreateVertexBuffer(float modelVertices[], unsigned int indices[], unsigned int sizeVertices,
                                   unsigned int sizeIndices)
{
    unsigned int triangleVAO;
    glGenVertexArrays(1, &triangleVAO);
    glBindVertexArray(triangleVAO);


    unsigned int triangleVBO;
    glGenBuffers(1, &triangleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeVertices, modelVertices, GL_STATIC_DRAW);

    unsigned int triangleEBO;
    glGenBuffers(1, &triangleEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 3 * sizeof(float) + 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    return Buffers{
        .VAO = triangleVAO,
        .VBO = triangleVBO,
    };
}

void DrawModel(ModelRenderInfo (&buffers)[2], float time)
{
    const float timeOffset = 0.032;
    for (int i = 0; i < 2; i++)
    {
        auto shaderBuffer = buffers[i];
        shaderBuffer.shaderProgram->Use();

        float opacity = sin(time + i * timeOffset);
        shaderBuffer.shaderProgram->setVariableFloat("opacity", opacity);


        glBindVertexArray(shaderBuffer.buffers.VAO);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
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

    float firstTriangleVertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
        0.5f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
    };

    unsigned int firstIndices[] = {
        0, 1, 2,
    };
    float secondTriangleVertices[] = {
        0.5f, -0.5f, 0.0f, 1.0f, 0.5f, 0.5f, 1.0f,
        1.0f, -0.5f, 0.0f, 0.5f, 1.0f, 0.5f, 1.0f,
        0.75f, 0.5f, 0.0f, 0.5f, 0.5f, 1.0f, 1.0f,
    };

    unsigned int secondIndices[] = {
        0, 1, 2,
    };

    auto buffersFirstTriangle = CreateVertexBuffer(firstTriangleVertices, firstIndices,
                                                           sizeof(firstTriangleVertices), sizeof(firstIndices));
    auto buffersSecondTriangle = CreateVertexBuffer(secondTriangleVertices, secondIndices,
                                                            sizeof(secondTriangleVertices), sizeof(secondIndices));

    Shader shader("Shaders/vertexShader.vert", "Shaders/fragmentShader.vert");

    ModelRenderInfo firstShaderBuffer = ModelRenderInfo{
        .buffers = buffersFirstTriangle, .shaderProgram = &shader
    };
    ModelRenderInfo secondShaderBuffer = ModelRenderInfo{
        .buffers = buffersSecondTriangle, .shaderProgram = &shader
    };

    ModelRenderInfo shaderBuffers[2] = {firstShaderBuffer, secondShaderBuffer};


    //std::cout << std::format("VBO id {}, VAO id {}", buffers.VBO, buffers.VAO);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        processInput(window);

        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        DrawModel(shaderBuffers, glfwGetTime());

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
