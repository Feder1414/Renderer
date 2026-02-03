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
#include "Scene.h"

AssimpLoader::AssimpLoader(Scene* scene, const std::string& filePath)
{
    m_scene = scene;
    m_filePath = filePath;
}

Entity* AssimpLoader::ImportScene()
{
    Assimp::Importer importer;
    const aiScene* aiScene = importer.ReadFile(m_filePath.c_str(),
                                               aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs);
    if (!aiScene || aiScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !aiScene->mRootNode)
    {
        std::cout << "ERROR ASSIMP" << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    m_aiScene = aiScene;
    ProcessNode(aiScene->mRootNode, aiScene, nullptr);
    return m_rootEntity;
}

void AssimpLoader::ProcessNode(aiNode* aiNode, const aiScene* aiScene, Entity* parentEntity)
{
    auto entity = std::make_unique<Entity>();

    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;

    AssimpLoader::DecomposeAiTranform(aiNode->mTransformation, pos, rot, scale);

    entity->SetAllLocal(pos, rot, scale);

    auto nodeName = std::string(aiNode->mName.C_Str());
    entity->SetName(nodeName);
    if (parentEntity)
    {
        parentEntity->AddEntityChild(entity.get());
    }
    else
    {
        m_rootEntity = entity.get();
    }


    // Process Mesh
    m_indexOffset = 0;
    m_vertexOffset = 0;
    if (aiNode->mMeshes)
    {
        auto meshName = m_filePath + nodeName;
        std::shared_ptr<Mesh> mesh = AssetManager::GetMesh(meshName);
        if (!mesh)
        {
            //Assumes every submesh has the same vertex layout
            std::shared_ptr<VertexLayout> vertexLayout = GetVertexLayout(aiScene->mMeshes[aiNode->mMeshes[0]]);
            std::vector<float> vertexData = {};
            std::vector<unsigned int> indexData = {};
            std::vector<SubMesh> submeshes(aiNode->mNumMeshes);

            std::vector<std::shared_ptr<Material>> materials = {};
            std::vector<SubMesh> subMeshes = {};
            //ProcessSubmeshes
            for (unsigned int meshIndex = 0; meshIndex < aiNode->mNumMeshes; meshIndex++)
            {
                auto meshSceneIndex = aiNode->mMeshes[meshIndex];
                auto aiMesh = aiScene->mMeshes[meshSceneIndex];


                ProcessSubmesh(aiMesh, meshName, vertexLayout, vertexData, indexData, submeshes, materials);
            }
            m_activeColorSets.clear();
            m_activeUvSets.clear();
            mesh = std::make_shared<Mesh>(vertexData, indexData, vertexLayout, submeshes, materials);
            AssetManager::AddMesh(meshName, mesh);
        }

        std::unique_ptr<ModelRenderInfo> renderInfo = std::make_unique<ModelRenderInfo>(mesh);

        entity->SetModelRenderInfo(std::move(renderInfo));
    }

    m_scene->AddEntity(std::move(entity));
}

void AssimpLoader::ProcessSubmesh(aiMesh* aiMesh, const std::string& meshName,
                                  std::shared_ptr<VertexLayout> vertexLayout,
                                  std::vector<float>& vertexData, std::vector<unsigned int>& indexData,
                                  std::vector<SubMesh>& submeshes, std::vector<std::shared_ptr<Material>>& materials)
{
    //Process mesh vertex data

    SubMesh submesh = {
        .indexOffset = m_indexOffset,
        .indexCount = 0,
        .vertexOffset = m_vertexOffset,
        .vertexCount = 0
    };

    for (unsigned int vertexIndex = 0; vertexIndex < aiMesh->mNumVertices; vertexIndex++)
    {
        //Load pos
        vertexData.push_back(aiMesh->mVertices->x);
        vertexData.push_back(aiMesh->mVertices->y);
        vertexData.push_back(aiMesh->mVertices->z);

        //Load color
        for (auto& activeChannelColor : m_activeColorSets)
        {
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].r);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].g);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].b);
            vertexData.push_back(aiMesh->mColors[activeChannelColor][vertexIndex].a);
        }
        //Load tex
        for (auto& activeUvChannel : m_activeUvSets)
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
        submesh.vertexCount += 1;
    }
    //Process mesh indexData
    for (unsigned int faceIndex = 0; faceIndex < aiMesh->mNumFaces; faceIndex++)
    {
        const auto& meshFace = aiMesh->mFaces[faceIndex];
        for (unsigned int indexIndex = 0; meshFace.mNumIndices; indexIndex++)
        {
            indexData.push_back(meshFace.mIndices[indexIndex]);
            submesh.indexCount += 1;
        }
    }

    submeshes.push_back(submesh);
    m_indexOffset += submesh.indexCount;
    m_vertexOffset += submesh.vertexCount;

    //Process Material of submesh
    aiMaterial* aiMaterial = m_aiScene->mMaterials[aiMesh->mMaterialIndex];
    std::string materialName = m_filePath + std::to_string(aiMesh->mMaterialIndex);
    std::shared_ptr<Material> material = AssetManager::GetMaterial(materialName);
    if (!material)
    {
        material = std::make_shared<Material>();

        std::vector<std::shared_ptr<Texture>> diffTextures;
        std::vector<std::shared_ptr<Texture>> specTextures;

        unsigned int diffLayers = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        unsigned specLayers = aiMaterial->GetTextureCount(aiTextureType_SPECULAR);

        if (diffLayers > 0)
        {
            std::cout << "The model with path " << m_filePath << " have the amount of " << std::to_string(diffLayers) <<
                " diffuse layers. Only one layer is supported" << std::endl;
        }
        if (specLayers > 0)
        {
            std::cout << "The model with path " << m_filePath << " have the amount of " << std::to_string(specLayers) <<
                " spec layers. Only one layer is supported" << std::endl;
        }


        ProcessTexture(aiMaterial, aiTextureType_DIFFUSE,
                       Material::MaterialPropertyNameToString(MaterialPropertyEnum::Diffuse), diffTextures);
        material->SetProperty(MaterialPropertyEnum::Diffuse, diffTextures[0]);

        AssetManager::AddMaterial(materialName, material);
    }
    materials.push_back(material);
}

std::shared_ptr<VertexLayout> AssimpLoader::GetVertexLayout(aiMesh* aiMesh)
{
    std::vector<VertexAttribute> vertexAttributes = {
        {
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::POSITION),
            .amountComponents = 3
        }

    };


    for (unsigned int i = 0; i < AI_MAX_NUMBER_OF_COLOR_SETS; i++)
    {
        if (aiMesh->HasVertexColors(i))
        {
            vertexAttributes.push_back({
                .type = VertexAttributeType::FLOAT,
                .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::COLOR, i),
                .amountComponents = 4
            });
            m_activeColorSets.push_back(i);
        }
    }


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
    return vertexLayout;
}

void AssimpLoader::TranslateAiTransformToGlTransform(aiMatrix4x4& aiTransform, glm::mat4& transform)
{
    aiTransform.Transpose();

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            transform[i][j] = aiTransform[i][j];
        }
    }
}

void AssimpLoader::DecomposeAiTranform(const aiMatrix4x4& aiTransform, glm::vec3& glmPosition, glm::vec3& glmRotation,
                                       glm::vec3& glmScale)
{
    aiVector3t<float> pos;
    aiVector3t<float> rot;
    aiVector3t<float> scale;

    aiTransform.Decompose(scale, rot, pos);

    AssimpLoader::CopyAiVectorToGlmVector(pos, glmPosition);
    AssimpLoader::CopyAiVectorToGlmVector(rot, glmRotation);
    AssimpLoader::CopyAiVectorToGlmVector(scale, glmScale);
}

void AssimpLoader::CopyAiVectorToGlmVector(const aiVector3t<float>& aiVector, glm::vec3& glmVec)
{
    glmVec.x = aiVector.x;
    glmVec.y = aiVector.y;
    glmVec.z = aiVector.z;
}

void AssimpLoader::ProcessTexture(aiMaterial* aiMaterial, aiTextureType aiTextureType, std::string typeName,
                                  std::vector<std::shared_ptr<Texture>>& textures)
{
    unsigned int amountTextures = aiMaterial->GetTextureCount(aiTextureType);
    for (int i = 0; i < amountTextures; i++)
    {
        aiString texPath;
        std::string texturePath(texPath.C_Str());
        aiMaterial->GetTexture(aiTextureType, i, &texPath);
        std::shared_ptr<Texture> texture = AssetManager::GetTexture(texturePath);
        if (!texture)
        {
            texture = std::make_shared<Texture>();
            texture->LoadImageFromFile(texturePath);
            AssetManager::AddTexture(texturePath, texture);
        }
        textures.push_back(texture);
    }
}
