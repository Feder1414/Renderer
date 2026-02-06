//
// Created by USUARIO on 1/30/2026.
//

#ifndef GRAFICOS_ASSIMPLOADER_H
#define GRAFICOS_ASSIMPLOADER_H
#include <string>

#include "Entity.h"
#include "assimp/material.h"
#include "assimp/matrix4x4.h"


class Entity;
struct aiMesh;
struct aiScene;
class aiNode;
class Mesh;

class Scene;

class AssimpLoader
{
public:
    AssimpLoader(Scene* scene, const std::string& filePath, std::shared_ptr<VertexLayout> targetVertexLayout = nullptr);
    Entity* ImportScene();

private:
    //Attr used for processiing submeshes in ProcessSubmes();
    unsigned int m_indexOffset = 0;
    unsigned int m_indexCount = 0;
    unsigned int m_vertexOffset = 0;
    unsigned int m_vertexCount = 0;
    const aiScene* m_aiScene;
    Entity* m_rootEntity;
    std::shared_ptr<VertexLayout> m_targetVertexLayout;


    std::vector<unsigned int> m_activeColorSets = {};
    std::vector<unsigned int> m_activeUvSets = {};

    std::string m_filePath;
    std::string m_localPath;
    void ProcessNode(aiNode* aiNode, const aiScene* scene, Entity*);
    void ProcessSubmesh(::aiMesh* aiMesh, const std::string& meshName,
                        std::shared_ptr<VertexLayout> vertexLayout, std::vector<float>& vertexData,
                        std::vector<unsigned int>& indexData, std::vector<SubMesh>& submeshes,
                        std::vector<std::shared_ptr<Material>>&
                        materials);
    void ProcessTexture(aiMaterial* aiMaterial, ::aiTextureType aiTextureType,
                        std::string typeName, std::vector<std::shared_ptr<Texture>>& textures);
    std::shared_ptr<Texture> ProcessMaterial();
    std::shared_ptr<VertexLayout> GetVertexLayout(aiMesh* aiMesh);


    static void TranslateAiTransformToGlTransform(aiMatrix4x4& aiTransform, glm::mat4& transform);
    static void DecomposeAiTranform(const aiMatrix4x4& aiTransform, glm::vec3& position, glm::vec3& rotation,
                                    glm::vec3& scale);
    static void CopyAiVectorToGlmVector(const aiVector3t<float>& aiVector, glm::vec3& glmVec);

    void CalculateBaseLocalPath();

    void CleanLoader();

    Scene* m_scene;
};


#endif //GRAFICOS_ASSIMPLOADER_H
