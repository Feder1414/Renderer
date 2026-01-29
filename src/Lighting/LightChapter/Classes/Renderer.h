//
// Created by USUARIO on 1/11/2026.
//

#ifndef GRAFICOS_RENDERER_H
#define GRAFICOS_RENDERER_H
#include <unordered_map>
#include <string>

#include "Material.h"

class Scene;
class Material;
class Shader;
class ModelRenderInfo;
class Entity;


class Renderer
{
public:
    void SetScene(Scene* scene) { m_currScene = scene; }
    void DebugRenderLights(bool renderLights) { m_renderLights = renderLights; }
    void RenderScene();

    void SetResolution(int width, int height)
    {
        m_width = width;
        m_height = height;
    }

private:
    int m_width, m_height = 0;
    bool m_renderLights = false;
    Scene* m_currScene;


    void UploadMaterialProperties(const Material* material, Shader* shader);

    void UploadUniformProperties(const std::unordered_map<std::string, UniformValue>& uniformValues,
                                 Shader* shader);
    void ChangeResolution(float m_width, float m_height);
    void UploadPerFrameProperties(Shader* shader);
    void UploadPerModelProperties(Shader* shader, const ModelRenderInfo* object);
    void UploadLightProperties(Shader* shader);
    void DrawModel(const ModelRenderInfo* object);
};


#endif //GRAFICOS_RENDERER_H
