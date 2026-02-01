//
// Created by USUARIO on 1/30/2026.
//

#include "AssetManager.h"

std::unordered_map<std::string, std::weak_ptr<Mesh>> AssetManager::m_Meshes = {};

std::shared_ptr<Mesh> AssetManager::AddMesh(const std::string& filePath, std::shared_ptr<Mesh> mesh)
{
    auto cacheMesh = m_Meshes.find(filePath);
    if (cacheMesh == m_Meshes.end())
    {
        m_Meshes[filePath] = mesh;
    }
    return m_Meshes[filePath].lock();
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


