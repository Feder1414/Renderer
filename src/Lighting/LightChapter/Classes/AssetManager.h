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
    static std::shared_ptr<Mesh> AddMesh(const std::string& meshName, std::shared_ptr<Mesh> mesh);
    static std::shared_ptr<Mesh> GetMesh(const std::string& filePath);

    static std::shared_ptr<Material> AddMaterial(const std::string& materialName, std::shared_ptr<Material> material);
    static std::shared_ptr<Material> GetMaterial(const std::string& materialName);

    static std::shared_ptr<Texture> AddTexture(std::shared_ptr<Texture> texture);
    static std::shared_ptr<Texture> GetTexture(const std::string& textureName);
};


#endif //GRAFICOS_ASSETMANAGER_H
