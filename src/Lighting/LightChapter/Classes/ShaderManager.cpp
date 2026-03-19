//
// Created by USUARIO on 2/7/2026.
//

#include "ShaderManager.h"
#include "Shader.h"

std::unordered_map<std::string, std::shared_ptr<Shader>> ShaderManager::shaderCache = {};

std::shared_ptr<Shader> ShaderManager::AddShader(std::shared_ptr<Shader>& shader)
{
    auto shaderKey = shader->GetShaderKey();
    if (shaderCache.contains(shaderKey))
    {
        return shaderCache[shaderKey];
    }
    shaderCache[shaderKey] = shader;
    return shaderCache[shaderKey];
}

std::shared_ptr<Shader> ShaderManager::GetShader(std::string& shaderKey)
{
    if (shaderCache.contains(shaderKey))
    {
        return shaderCache[shaderKey];
    }
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::GetDefaultShader(DefaultShader defaultShader, int permutation)
{
    auto shaderKey = DefaultShaderToStr(defaultShader, permutation);
    if (shaderCache.contains(shaderKey))
    {
        return shaderCache[shaderKey];
    }
    return nullptr;
}

std::shared_ptr<Shader> ShaderManager::AddDefaultShader(std::shared_ptr<Shader>& shader, DefaultShader defaultShader,
                                                        int permutation)
{
    auto shaderKey = ShaderManager::DefaultShaderToStr(defaultShader, permutation);
    if (shaderCache.contains(shaderKey))
    {
        return shaderCache[shaderKey];
    }
    shaderCache[shaderKey] = shader;

    return shaderCache[shaderKey];
}


std::string ShaderManager::DefaultShaderToStr(DefaultShader defaultShader, int permutation)
{
    switch (defaultShader)
    {
    case DefaultShader::BorderColor: return "borderColor";
    case DefaultShader::AABB: return "aabb";
    case DefaultShader::FullScreenQuad: return "fullScreenQuad";
    case DefaultShader::NormalViewer: return "normalViewer" + std::to_string(permutation);
    default:
        return "Error";
    }
}
