//
// Created by USUARIO on 2/7/2026.
//

#ifndef GRAFICOS_SHADERMANAGER_H
#define GRAFICOS_SHADERMANAGER_H
#include <memory>
#include <string>
#include <unordered_map>

class Shader;

enum class DefaultShader
{
    BorderColor,
    AABB,
    NormalViewer,
    FullScreenQuad,
    Skybox,
    ShadowPass,
    ShadowPassInstancing,
    ShadowPassDebug,
    FrustumViewer,
};

class ShaderManager
{
public:
    static std::shared_ptr<Shader> AddShader(std::shared_ptr<Shader>& shader);
    static std::shared_ptr<Shader> GetShader(std::string& shaderKey);
    static std::shared_ptr<Shader> AddDefaultShader(std::shared_ptr<Shader>& shader, DefaultShader defaultShader,
                                                    int permutation = 0);
    static std::string DefaultShaderToStr(DefaultShader defaultShader, int permutation = 0);
    static std::shared_ptr<Shader> GetDefaultShader(DefaultShader defaultShader, int permutation = 0);

private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> shaderCache;
};


#endif //GRAFICOS_SHADERMANAGER_H
