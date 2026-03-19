//
// Created by USUARIO on 2/7/2026.
//

#include "BasicShapesMeshGenerator.h"
#include  <vector>
#include "Mesh.h"
#include "Material.h"
#include "BV/AABB.h"

std::shared_ptr<Mesh> BasicShapesMeshGenerator::CreateCubeMesh(std::shared_ptr<Material> material,
                                                               std::shared_ptr<VertexLayout> vertexLayout)
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
        2, 1, 0,
        5, 4, 3,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        20, 19, 18,
        23, 22, 21,
        24, 25, 26,
        27, 28, 29,
        32, 31, 30,
        35, 34, 33


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

    std::shared_ptr<Mesh> cubeMesh = std::make_shared<Mesh>("Cube", cubeVertices, cubeIndices, vertexLayout, submeshes,
                                                            submeshToMaterial);

    auto aabb = std::make_unique<AABB>(glm::vec3(-0.5f, -0.5f, -0.5), glm::vec3(0.5f, 0.5f, 0.5f));
    cubeMesh->SetBoundingVolume(std::move(aabb));

    return cubeMesh;
}

std::shared_ptr<Mesh> BasicShapesMeshGenerator::CreateQuad(std::shared_ptr<Material> material,
                                                           std::shared_ptr<VertexLayout> vertexLayout)
{
    std::vector<float> quadData = {
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    };

    // std::vector<float> quadPos = {
    //     -0.5f, -0.5f, 0.5f,
    //     0.5f, -0.5f, 0.5f,
    //     0.5f, 0.5f, 0.5f,
    //     0.5f, 0.5f, 0.5f,
    //     -0.5f, 0.5f, 0.5f,
    //     -0.5f, -0.5f, 0.5f,
    //
    // };
    //
    // std::vector<float> quadColor = {
    //     0.5f, 0.5f, 0.5f, 1.0f,
    //     0.5f, 0.5f, 0.5f, 1.0f,
    //     0.5f, 0.5f, 0.5f, 1.0f,
    //     0.5f, 0.5f, 0.5f, 1.0f,
    //     0.5f, 0.5f, 0.5f, 1.0f,
    //     0.5f, 0.5f, 0.5f, 1.0f,
    // };

    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        3, 4, 5,
    };


    std::vector<SubMesh> submeshes = {
    };
    submeshes.push_back({
        .indexOffset = 0,
        .indexCount = static_cast<unsigned int>(quadIndices.size()),

        .vertexOffset = 0,
        .vertexCount = static_cast<unsigned int>(quadData.size())

    });
    std::vector<std::shared_ptr<Material>> submeshToMaterial = {material};

    std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>("Quad", quadData, quadIndices, vertexLayout, submeshes,
                                                            submeshToMaterial);

    auto aabb = std::make_unique<AABB>(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.6f));
    quadMesh->SetBoundingVolume(std::move(aabb));

    return quadMesh;
}

std::shared_ptr<Mesh> BasicShapesMeshGenerator::CreateFullScreenQuad()
{
    std::vector<float> quadData = {
        // pos                  // uv
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f
    };


    std::vector<VertexAttribute> vertexAttribs = {
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::POSITION),
            .amountComponents = 3
        },
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::UV),
            .amountComponents = 2
        }

    };

    auto vertexLayout = std::make_shared<VertexLayout>(vertexAttribs);

    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        3, 4, 5,
    };


    std::vector<SubMesh> submeshes = {
    };
    submeshes.push_back({
        .indexOffset = 0,
        .indexCount = static_cast<unsigned int>(quadIndices.size()),

        .vertexOffset = 0,
        .vertexCount = static_cast<unsigned int>(quadData.size())

    });
    std::vector<std::shared_ptr<Material>> submeshToMaterial = {nullptr};

    std::shared_ptr<Mesh> quadMesh = std::make_shared<Mesh>("FullScreenQuad", quadData, quadIndices, vertexLayout,
                                                            submeshes,
                                                            submeshToMaterial);

    auto aabb = std::make_unique<AABB>(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.5f, 0.5f, 0.6f));
    quadMesh->SetBoundingVolume(std::move(aabb));

    return quadMesh;
}

std::shared_ptr<Mesh> BasicShapesMeshGenerator::CreateSkyboxCube()
{
    std::vector<float> skyboxVertices = {
        // positions
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f
    };

    std::vector<unsigned int> cubeIndices = {
        2, 1, 0,
        5, 4, 3,
        6, 7, 8,
        9, 10, 11,
        12, 13, 14,
        15, 16, 17,
        20, 19, 18,
        23, 22, 21,
        24, 25, 26,
        27, 28, 29,
        32, 31, 30,
        35, 34, 33


    };

    std::vector<VertexAttribute> vertexAttribs = {
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::POSITION),
            .amountComponents = 3
        },
    };

    auto vertexLayout = std::make_shared<VertexLayout>(vertexAttribs);


    std::vector<SubMesh> submeshes = {
    };
    submeshes.push_back({
        .indexOffset = 0,
        .indexCount = static_cast<unsigned int>(cubeIndices.size()),

        .vertexOffset = 0,
        .vertexCount = static_cast<unsigned int>(skyboxVertices.size())

    });
    std::vector<std::shared_ptr<Material>> submeshToMaterial = {nullptr};


    std::shared_ptr<Mesh> skyboxMesh = std::make_shared<Mesh>("SkyBoxMesh", skyboxVertices, cubeIndices, vertexLayout,
                                                              submeshes,
                                                              submeshToMaterial);

    return skyboxMesh;
}
