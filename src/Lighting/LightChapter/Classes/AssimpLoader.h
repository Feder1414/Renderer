//
// Created by USUARIO on 1/30/2026.
//

#ifndef GRAFICOS_ASSIMPLOADER_H
#define GRAFICOS_ASSIMPLOADER_H
#include <string>

#include "Entity.h"


class Entity;
struct aiMesh;
struct aiScene;
class aiNode;
class Mesh;

class Scene;

class AssimpLoader
{
public:
    AssimpLoader(Scene* scene, const std::string& filePath);
    void ImportScene();



private:
    std::string m_filePath;
    void ProcessNode(aiNode* aiNode, const aiScene* scene, Entity*);
    std::shared_ptr<Mesh> ProcessMesh(::aiMesh* aiMesh, const std::string& meshName);
    std::shared_ptr<Texture>  ProcessMaterial
    Scene* m_scene;




};


#endif //GRAFICOS_ASSIMPLOADER_H