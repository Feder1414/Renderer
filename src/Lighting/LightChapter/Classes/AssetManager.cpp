//
// Created by USUARIO on 1/30/2026.
//

#include "AssetManager.h"

#include "Texture.h"

std::unordered_map<std::string, std::weak_ptr<Mesh>> AssetManager::m_Meshes = {};
std::unordered_map<std::string, std::weak_ptr<Material>> AssetManager::m_Materials = {};
std::unordered_map<std::string, std::weak_ptr<Texture>> AssetManager::m_Textures = {};

std::shared_ptr<Mesh> AssetManager::AddMesh(const std::string& meshName, std::shared_ptr<Mesh> mesh)
{
    auto cacheMesh = m_Meshes.find(meshName);
    if (cacheMesh == m_Meshes.end())
    {
        m_Meshes[meshName] = mesh;
    }
    return m_Meshes[meshName].lock();
}

std::shared_ptr<Mesh> AssetManager::GetMesh(const std::string& filePath)
{
    auto cacheMesh = m_Meshes.find(filePath);
    if (cacheMesh == m_Meshes.end())
    {
        return nullptr;
    }
    return m_Meshes[filePath].lock();
}

std::shared_ptr<Material> AssetManager::GetMaterial(const std::string& materialName)
{
    auto cacheMaterial = m_Meshes.find(materialName);
    if (cacheMaterial == m_Meshes.end())
    {
        return nullptr;
    }
    return m_Materials[materialName].lock();
}

std::shared_ptr<Material> AssetManager::AddMaterial(const std::string& materialName, std::shared_ptr<Material> material)
{
    auto cacheMaterial = m_Meshes.find(materialName);
    if (cacheMaterial == m_Meshes.end())
    {
        m_Materials[materialName] = material;
    }
    return m_Materials[materialName].lock();
}

std::shared_ptr<Texture> AssetManager::GetTexture(const std::string& textureName)
{
    auto cacheTexture = m_Textures.find(textureName);
    if (cacheTexture == m_Textures.end())
    {
        return nullptr;
    }
    return m_Textures[textureName].lock();
}

std::shared_ptr<Texture> AssetManager::AddTexture(std::shared_ptr<Texture> texture)
{
    auto texturePath = texture->GetFilePath();
    auto cacheTexture = m_Textures.find(texturePath);
    if (cacheTexture == m_Textures.end())
    {
        m_Textures[texturePath] = texture;
    }
    return m_Textures[texturePath].lock();
}
