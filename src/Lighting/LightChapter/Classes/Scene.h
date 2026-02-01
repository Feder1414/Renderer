//
// Created by USUARIO on 1/11/2026.
//

#ifndef GRAFICOS_SCENE_H
#define GRAFICOS_SCENE_H
#include <vector>


#include "Light.h"
#include "Entity.h"
#include "ModelRenderInfo.h"
#include "Camera.h"

struct SceneLight
{
    glm::vec3 ambientColor = glm::vec3(1.0f, 1.0f, 1.0f);
    float ambientFactor = 0.1f;
};


class Scene
{
private:
    std::unique_ptr<SceneLight> m_sceneLight = std::make_unique<SceneLight>();
    std::vector<std::unique_ptr<Entity>> m_entities = {};
    std::vector<Entity*> m_lights = {};
    std::vector<std::unique_ptr<Camera>> m_cameras = {};
    int m_indexActiveCamera = 0;

public:
    void AddEntity(std::unique_ptr<Entity> entity);

    void AddCamera(std::unique_ptr<Camera> camera, bool setActive = true)
    {
        m_cameras.push_back(std::move(camera));
        if (setActive)
        {
            m_indexActiveCamera = m_cameras.size() - 1;
        }
    }

    const std::vector<Entity*>& GetLights() const
    {
        return m_lights;
    }

    const std::vector<std::unique_ptr<Entity>>& GetEntities() const
    {
        return m_entities;
    }

    const std::vector<std::unique_ptr<Camera>>& GetCameras()
    {
        return m_cameras;
    }

    const SceneLight* GetSceneLight() const
    {
        return m_sceneLight.get();
    }

    Camera* GetActiveCamera() const
    {
        if (m_cameras.empty())
        {
            std::cout << "There are no camera active because there are no cameras" << std::endl;
        }
        return m_cameras[m_indexActiveCamera].get();
    }
};


#endif //GRAFICOS_SCENE_H
