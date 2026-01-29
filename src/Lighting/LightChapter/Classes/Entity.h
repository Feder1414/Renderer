//
// Created by USUARIO on 1/23/2026.
//

#ifndef GRAFICOS_GAMEOBJECTS_H
#define GRAFICOS_GAMEOBJECTS_H
#include <memory>

#include "uuid.h"
#include "Transform.h"


class IComponent;
class Transform;
class ModelRenderInfo;
class Entity;

using FunctionUpdateEntity = std::function<void(Entity*, float deltaTime)>;

class Entity
{
    uuids::uuid m_id;

    std::unique_ptr<ModelRenderInfo> m_modelRenderInfo;
    FunctionUpdateEntity m_updateFunction = [](Entity* entity, float deltaTime)
    {
    };
    std::vector<std::unique_ptr<IComponent>> m_components = {};

public:
    void SetModelRenderInfo(std::unique_ptr<ModelRenderInfo> modelRenderInfo);

    void SetId(uuids::uuid id) { m_id = id; }

    void Update(float deltaTime)
    {
        m_updateFunction(this, deltaTime);
    }

    void SetUpdate(const FunctionUpdateEntity& functionUpdate)
    {
        m_updateFunction = functionUpdate;
    }

    void AddComponent(std::unique_ptr<IComponent> component);

    template <typename ComponentName>
    ComponentName* GetComponent()
    {
        for (auto& component : m_components)
        {
            if (auto casted = dynamic_cast<ComponentName*>(component.get()))
            {
                return casted;
            }
        }
        return nullptr;
    };

    Transform transform = Transform();


    ModelRenderInfo* GetModelRenderInfo() const;
};


#endif //GRAFICOS_GAMEOBJECTS_H
