//
// Created by USUARIO on 1/30/2026.
//

#include "AssimpLoader.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "AssetManager.h"
#include "Entity.h"

AssimpLoader::AssimpLoader(Scene* scene, const std::string& filePath)
{
    m_scene = scene;
    m_filePath = filePath;
}

void AssimpLoader::ImportScene()
{
    Assimp::Importer importer;
    const aiScene* aiScene = importer.ReadFile(m_filePath.c_str(),
                                               aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        std::cout << "ERROR ASSIMP" << importer.GetErrorString() << std::endl;
        return;
    }
    ProcessNode(aiScene->mRootNode, aiScene, nullptr);
}

void AssimpLoader::ProcessNode(aiNode* aiNode, const aiScene* aiScene, Entity* parentEntity)
{
    auto entity = std::make_unique<Entity>();
    auto nodeName = std::string(aiNode->mName.C_Str());
    entity->SetName(nodeName);
    if (parentEntity)
    {
        parentEntity->AddEntityChild(entity.get());
    }

    for (unsigned int meshIndex = 0; meshIndex < aiNode->mNumMeshes; meshIndex++)
    {
        auto aiMesh = aiScene->mMeshes[meshIndex];
        auto meshName = m_filePath + nodeName;

        auto mesh = ProcessMesh(aiMesh, meshName);
    }
}

std::shared_ptr<Mesh> AssimpLoader::ProcessMesh(aiMesh* aiMesh, const std::string& meshName)
{
    auto meshCache = AssetManager::GetMesh(meshName);
    if (meshCache)
    {
        return meshCache;
    }
    std::vector<VertexAttribute> vertexAttributes = {
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::POSITION),
            .amountComponents = 3
        }

    };

    std::vector<unsigned int> activeColorSets = {};

    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++)
    {
        if (aiMesh->HasVertexColors(i))
        {
            vertexAttributes.push_back({
                .type = VertexAttributeType::FLOAT,
                .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::COLOR, i),
                .amountComponents = 4
            });
            activeColorSets.push_back(i);
        }
    }

    std::vector<unsigned int> activeUvSets = {};

    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_TEXTURECOORDS; i++)
    {
        if (aiMesh->mTextureCoords[i])
        {
            auto amountComponentsChannel = aiMesh->mNumUVComponents[i];
            vertexAttributes.push_back({
                .type = VertexAttributeType::FLOAT,
                .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::UV, i),
                .amountComponents = static_cast<int>(amountComponentsChannel),
            });
            activeUvSets.push_back(i);
        }
    }


    if (aiMesh->mNormals)
    {
        vertexAttributes.push_back({
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::NORMAL),
            .amountComponents = 3
        });
    }

    if (aiMesh->mTangents)
    {
        vertexAttributes.push_back({
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::TANGENT),
            .amountComponents = 3
        });
    }
    std::shared_ptr<VertexLayout> vertexLayout = std::make_shared<VertexLayout>(vertexAttributes);
    std::vector<float> vertexData(aiMesh->mNumVertices * vertexLayout->GetComponentsPerVertex());
    std::vector<int> indexData(aiMesh->mNumVertices);

    //Process mesh vertex data
    for (unsigned int vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
    {
        //Load pos
        vertexData.push_back(aiMesh->mVertices->x);
        vertexData.push_back(aiMesh->mVertices->y);
        vertexData.push_back(aiMesh->mVertices->z);

        //Load color
        for (auto& activeChannelColor : activeColorSets)
        {
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].r);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].g);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].b);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].a);
        }
        //Load tex
        for (auto& activeUvChannel : activeUvSets)
        {
            auto amountComponents = aiMesh->mNumUVComponents[activeUvChannel];
            for (unsigned int p = 0; vertexIndex < amountComponents; vertexIndex++)
            {
                vertexData.push_back(aiMesh->mTextureCoords[activeUvChannel][vertexIndex][p]);
            }
        }

        //Load normal
        if (aiMesh->mNormals)
        {
            vertexData.push_back(aiMesh->mNormals[vertexIndex].x);
            vertexData.push_back(aiMesh->mNormals[vertexIndex].y);
            vertexData.push_back(aiMesh->mNormals[vertexIndex].z);
        }

        if (aiMesh->mTangents)
        {
            vertexData.push_back(aiMesh->mTangents[vertexIndex].x);
            vertexData.push_back(aiMesh->mTangents[vertexIndex].y);
            vertexData.push_back(aiMesh->mTangents[vertexIndex].z);
        }
    }

    //Process mesh indexData
    for (unsigned int faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++)
    {
        const auto& meshFace = aiMesh->mFaces[faceIndex];
        for (unsigned int indexIndex = 0; meshFace.mNumIndices; indexIndex++)
        {
            indexData.push_back(meshFace.mIndices[indexIndex]);
        }
    }
    std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(vertexData, indexData, vertexLayout);
    AssetManager::AddMesh(meshName, mesh);
    return mesh;
}
