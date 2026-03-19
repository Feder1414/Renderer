//
// Created by USUARIO on 1/11/2026.
//

#ifndef GRAFICOS_SCENE_H
#define GRAFICOS_SCENE_H
#include <iostream>
#include <vector>


#include "Entity.h"
#include "Skybox.h"


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
    std::vector<Entity*> m_cameras = {};
    int m_indexActiveCamera = 0;
    std::vector<Entity*> m_rootEntities;

    std::unique_ptr<Skybox> m_skybox = nullptr;

public:
    void AddEntity(std::unique_ptr<Entity> entity);

    // void AddCamera(std::unique_ptr<Camera> camera, bool setActive = true)
    // {
    // }

    const std::vector<Entity*>& GetLights() const
    {
        return m_lights;
    }

    const std::vector<std::unique_ptr<Entity>>& GetEntities() const
    {
        return m_entities;
    }

    const std::vector<Entity*>& GetCameras()
    {
        return m_cameras;
    }

    const SceneLight* GetSceneLight() const
    {
        return m_sceneLight.get();
    }

    Entity* GetActiveCamera() const
    {
        if (m_cameras.empty())
        {
            std::cout << "There are no camera active because there are no cameras" << std::endl;
        }
        return m_cameras[m_indexActiveCamera];
    }

    void SetSkyBox(std::unique_ptr<Skybox> skybox) { m_skybox = std::move(skybox); }

    Skybox* GetSkyBox() const { return m_skybox.get(); }

    std::vector<Entity*>& GetSceneTree() { return m_rootEntities; }
};


#endif //GRAFICOS_SCENE_H
