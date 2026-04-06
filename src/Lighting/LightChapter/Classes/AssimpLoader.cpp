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
#include "BV/AABB.h"

void AssimpLoader::SetLoadInfo(Scene* scene, const std::string& filePath,
                               std::shared_ptr<VertexLayout> targetVertexLayout)
{
    m_scene = scene;
    m_filePath = filePath;
    m_targetVertexLayout = targetVertexLayout;
    CalculateBaseLocalPath();
}

AssimpLoader::AssimpLoader()
{
}

Entity* AssimpLoader::ImportScene()
{
    if (!m_scene)
    {
        std::cout << "There is no scene currently bound, you must first execute the function SetLoadInfo" << std::endl;
        return nullptr;
    }
    Assimp::Importer importer;
    const aiScene* aiScene = importer.ReadFile(m_filePath.c_str(),
                                               aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_FlipUVs |
                                               aiProcess_GenBoundingBoxes | aiProcess_GenSmoothNormals);
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
    if (aiNode->mNumMeshes > 0)
    {
        auto meshName = m_filePath + nodeName;
        std::shared_ptr<Mesh> mesh = AssetManager::GetMesh(meshName);

        if (!mesh)
        {
            //Assumes every submesh has the same vertex layout
            std::shared_ptr<VertexLayout> vertexLayout = GetVertexLayout(aiScene->mMeshes[aiNode->mMeshes[0]]);

            std::vector<float> vertexData = {};
            std::vector<unsigned int> indexData = {};
            std::vector<SubMesh> submeshes = {};

            std::vector<std::shared_ptr<Material>> materials = {};
            //ProcessSubmeshes
            glm::vec3 minAABB;
            glm::vec3 maxAABB;
            for (unsigned int meshIndex = 0; meshIndex < aiNode->mNumMeshes; meshIndex++)
            {
                auto meshSceneIndex = aiNode->mMeshes[meshIndex];
                auto aiMesh = aiScene->mMeshes[meshSceneIndex];
                auto aiAABB = aiMesh->mAABB;
                if (meshIndex == 0)
                {
                    minAABB = glm::vec3(aiAABB.mMin.x, aiAABB.mMin.y, aiAABB.mMin.z);
                    maxAABB = glm::vec3(aiAABB.mMax.x, aiAABB.mMax.y, aiAABB.mMax.z);
                }
                minAABB.x = std::min(aiAABB.mMin.x, minAABB.x);
                minAABB.y = std::min(aiAABB.mMin.y, minAABB.y);
                minAABB.z = std::min(aiAABB.mMin.z, minAABB.z);
                maxAABB.x = std::max(aiAABB.mMax.x, maxAABB.x);
                maxAABB.y = std::max(aiAABB.mMax.y, maxAABB.y);
                maxAABB.z = std::max(aiAABB.mMax.z, maxAABB.z);


                ProcessSubmesh(aiMesh, meshName, vertexLayout, vertexData, indexData, submeshes, materials);
            }
            m_activeColorSets.clear();
            m_activeUvSets.clear();
            mesh = std::make_shared<Mesh>(meshName, vertexData, indexData, vertexLayout, submeshes, materials);
            if (m_targetVertexLayout)
            {
                mesh->SetTargetVertexLayout(m_targetVertexLayout);
            }
            std::unique_ptr<AABB> aabb = std::make_unique<AABB>(minAABB, maxAABB);
            mesh->SetBoundingVolume(std::move(aabb));
            AssetManager::AddMesh(meshName, mesh);
        }


        std::unique_ptr<ModelRenderInfo> renderInfo = std::make_unique<ModelRenderInfo>(mesh);

        entity->SetModelRenderInfo(std::move(renderInfo));
    }


    for (int i = 0; i < aiNode->mNumChildren; i++)
    {
        ProcessNode(aiNode->mChildren[i], aiScene, entity.get());
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
        vertexData.push_back(aiMesh->mVertices[vertexIndex].x);
        vertexData.push_back(aiMesh->mVertices[vertexIndex].y);
        vertexData.push_back(aiMesh->mVertices[vertexIndex].z);

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
            for (unsigned int p = 0; p < amountComponents; p++)
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
        for (unsigned int indexIndex = 0; indexIndex < meshFace.mNumIndices; indexIndex++)
        {
            indexData.push_back(meshFace.mIndices[indexIndex] + submesh.vertexOffset);
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
        material = std::make_shared<Material>(materialName);

        std::vector<std::shared_ptr<Texture>> baseColTextures;
        std::vector<std::shared_ptr<Texture>> specTextures;


        unsigned int diffLayers = aiMaterial->GetTextureCount(aiTextureType_DIFFUSE);
        unsigned int albedoLayers = aiMaterial->GetTextureCount(aiTextureType_BASE_COLOR);
        unsigned int unknowLayers = aiMaterial->GetTextureCount(aiTextureType_UNKNOWN);


        unsigned specLayers = aiMaterial->GetTextureCount(aiTextureType_SPECULAR);
        auto amountTotalTextures = 0;

        float shininess;

        if (aiGetMaterialFloat(aiMaterial, AI_MATKEY_SHININESS, &shininess) != AI_SUCCESS)
        {
            shininess = 32.0f;
        }

        material->SetProperty(MaterialPropertyEnum::Shininess, static_cast<int>(shininess));

        aiColor3D diffuseColor;
        if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
        {
            glm::vec3 glmColor = glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b);
            material->SetProperty(MaterialPropertyEnum::Diffuse, glmColor);
        }


        // for (unsigned int i = 1; i < aiTextureType_GLTF_METALLIC_ROUGHNESS + 1; i++)
        // {
        //     auto textureType = static_cast<aiTextureType>(i);
        //     amountTotalTextures += aiMaterial->GetTextureCount(textureType);
        // }
        //
        // if (amountTotalTextures > 0)
        // {
        //     std::cout << "THere are  fucking texturesssssssssssssssssssssssss" << std::endl;
        // }

        if (diffLayers > 0)
        {
            std::cout << "The model with path " << m_filePath << " have the amount of " << std::to_string(diffLayers) <<
                " diffuse layers. Only one layer is supported" << std::endl;
        }
        if (specLayers > 0)
        {
            std::cout << "The model with path " << m_filePath << " have the amount of " << std::to_string(specLayers) <<
                " spec layers. Only one layer is supported" << std::endl;

            ProcessTexture(aiMaterial, aiTextureType_SPECULAR,
                           Material::MaterialPropertyNameToString(MaterialPropertyEnum::Specular), specTextures, false);
            material->SetProperty(MaterialPropertyEnum::Specular, specTextures[0]);
            material->SetProperty(MaterialPropertyEnum::HasSpecularTexture, true);
        }


        if (albedoLayers > 0)
        {
            ProcessTexture(aiMaterial, aiTextureType_BASE_COLOR,
                           Material::MaterialPropertyNameToString(MaterialPropertyEnum::Diffuse), baseColTextures,
                           true);
        }
        else if (diffLayers > 0)
        {
            ProcessTexture(aiMaterial, aiTextureType_DIFFUSE,
                           Material::MaterialPropertyNameToString(MaterialPropertyEnum::Diffuse), baseColTextures,
                           true);
        }


        if (!baseColTextures.empty())
        {
            material->SetProperty(MaterialPropertyEnum::Diffuse, baseColTextures[0]);
        }


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
            m_activeUvSets.push_back(i);
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

    glmRotation = glm::degrees(glmRotation);
}

void AssimpLoader::CopyAiVectorToGlmVector(const aiVector3t<float>& aiVector, glm::vec3& glmVec)
{
    glmVec.x = aiVector.x;
    glmVec.y = aiVector.y;
    glmVec.z = aiVector.z;
}

void AssimpLoader::ProcessTexture(aiMaterial* aiMaterial, aiTextureType aiTextureType, std::string typeName,
                                  std::vector<std::shared_ptr<Texture>>& textures, bool isSRGB)
{
    unsigned int amountTextures = aiMaterial->GetTextureCount(aiTextureType);


    for (int i = 0; i < amountTextures; i++)
    {
        aiString texPath;
        aiMaterial->GetTexture(aiTextureType, i, &texPath);
        std::string texturePath(texPath.C_Str());
        texturePath = m_localPath + texturePath;
        std::shared_ptr<Texture> texture = AssetManager::GetTexture(texturePath);
        if (texture)
        {
            textures.push_back(texture);
            continue;
        }
        texture = std::make_shared<Texture>();
        const aiTexture* embededTexture = m_aiScene->GetEmbeddedTexture(texPath.C_Str());

        if (embededTexture)
        {
            if (embededTexture->mHeight == 0)
            {
                texture->LoadImageRaw(texturePath, embededTexture->mWidth,
                                      reinterpret_cast<const unsigned char*>(embededTexture->pcData), true, isSRGB);
            }
            else
            {
                // I dont know what to do yet xd
            }
        }
        else
        {
            texture->LoadImageFromFile(texturePath, true, isSRGB);
        }

        textures.push_back(texture);


        AssetManager::AddTexture(texture);
    }
}

void AssimpLoader::CalculateBaseLocalPath()
{
    m_localPath = m_filePath;
    auto lastIndexSlash = m_filePath.rfind("/");

    if (lastIndexSlash != std::string::npos)
    {
        m_localPath = m_localPath.substr(0, lastIndexSlash + 1);
    }
}

void AssimpLoader::CleanLoader()
{
    m_aiScene = nullptr;
    m_rootEntity = nullptr;
    m_targetVertexLayout = nullptr;
    m_activeColorSets.clear();
    m_activeColorSets.clear();
    m_filePath = "";
    m_localPath = "";
    m_scene = nullptr;
}
