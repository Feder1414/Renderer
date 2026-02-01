//
// Created by USUARIO on 1/30/2026.
//

#ifndef GRAFICOS_ASSETMANAGER_H
#define GRAFICOS_ASSETMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

#include "Mesh.h"


class Texture;
class Material;

class AssetManager
{
    static std::unordered_map<std::string, std::weak_ptr<Mesh>> m_Meshes;
    static std::unordered_map<std::string, std::weak_ptr<Material>> m_Materials;
    static std::unordered_map<std::string, std::weak_ptr<Texture>> m_Textures;

public:
    static std::shared_ptr<Mesh> AddMesh(const std::string& filePath, std::shared_ptr<Mesh> mesh);

    static std::shared_ptr<Mesh> GetMesh(const std::string& filePath);
};


#endif //GRAFICOS_ASSETMANAGER_H
