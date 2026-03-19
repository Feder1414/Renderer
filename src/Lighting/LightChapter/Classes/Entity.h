//
// Created by USUARIO on 1/23/2026.
//

#ifndef GRAFICOS_GAMEOBJECTS_H
#define GRAFICOS_GAMEOBJECTS_H
#include <memory>

#include "uuid.h"
#include "Transform.h"
#include "../Components/ModelRenderInfo.h"


class Scene;
class IComponent;
class Transform;
class Entity;

using FunctionUpdateEntity = std::function<void(Entity*, float deltaTime)>;

class Entity
{
    uuids::uuid m_id;
    std::string m_name;

    Entity* m_parent = nullptr;

    std::unique_ptr<ModelRenderInfo> m_modelRenderInfo;
    FunctionUpdateEntity m_updateFunction = [](Entity* entity, float deltaTime)
    {
    };
    std::vector<std::unique_ptr<IComponent>> m_components = {};

    std::vector<Entity*> m_child = {};

    Scene* m_scene;

    void SetEntityParent(Entity* entity) { m_parent = entity; }
    void UpdateTransform();

    glm::mat4 GetRotation() const;

    static void CopyMembersEntity(Entity* original, Entity* copyEntity);
    Entity* CopyChildren(Entity* entity, std::vector<std::unique_ptr<Entity>>& childEntities, int k);
    void CalculateLocalMatrix();


    //Transform
    glm::vec3 m_localPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_localRot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_localScale = glm::vec3(1.0f, 1.0f, 1.0f);

    glm::vec3 m_worldPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_worldRot = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_worldScale = glm::vec3(0.0f, 0.0f, 0.0f);


    glm::mat4 m_localMat = glm::mat4(1.0f);
    glm::mat4 m_worldMat = glm::mat4(1.0f);

    glm::mat3 m_normMat = glm::mat3(1.0f);


    glm::vec3 m_forward = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 m_up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 m_right = glm::vec3(1.0f, 0.0f, 0.0f);


    //World


public:
    Entity();
    void SetModelRenderInfo(std::unique_ptr<ModelRenderInfo> modelRenderInfo);

    void SetId(uuids::uuid id) { m_id = id; }

    uuids::uuid GetId() { return m_id; }
    [[nodiscard]] std::string GetIdStr() const { return to_string(m_id); }

    void SetScene(Scene* scene);

    void SetName(const std::string& name) { m_name = name; }
    std::string& GetName() { return m_name; }

    Scene* GetScene() const { return m_scene; }

    void Update(float deltaTime)
    {
        for (auto& component : m_components)
        {
            component->Update();
        }
        m_updateFunction(this, deltaTime);
    }

    void SetUpdate(const FunctionUpdateEntity& functionUpdate)
    {
        m_updateFunction = functionUpdate;
    }

    void SetPosGlobal(glm::vec3 globalPos)
    {
        if (!m_parent)
        {
            SetLocalPos(glm::vec3(globalPos));
            return;
        }
        auto localPos = inverse(m_parent->GetWorldMat()) * glm::vec4(globalPos, 1.0f);
        SetLocalPos(glm::vec3(localPos));
    }

    void ApplyFunctionToChildren(const std::function<void(Entity*)>& function)
    {
        function(this);
        for (auto child : m_child)
        {
            child->ApplyFunctionToChildren(function);
        }
    };


    void AddComponent(std::unique_ptr<IComponent> component);

    void AddEntityChild(Entity* childEntity)
    {
        m_child.push_back(childEntity);
        childEntity->SetEntityParent(this);
        childEntity->UpdateTransform();
    }


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

    Entity* GetParent() const
    {
        return m_parent;
    }

    std::vector<Entity*>& GetChild() { return m_child; }


    void SetLocalPos(glm::vec3 pos)
    {
        m_localPos = pos;

        UpdateTransform();
    }

    void SetLocalRot(const glm::vec3 rot)
    {
        m_localRot = rot;
        UpdateTransform();
    }

    void SetLocalScale(glm::vec3 scale)
    {
        m_localScale = scale;
        UpdateTransform();
    }

    const glm::vec3& GetLocalPos() const { return m_localPos; }
    const glm::vec3& GetLocalRot() const { return m_localRot; }
    const glm::vec3& GetLocalScale() const { return m_localScale; }


    void SetAllLocal(const glm::vec3& localPos, const glm::vec3& localRot, const glm::vec3 localScale)
    {
        m_localPos = localPos;
        m_localRot = localRot;
        m_localScale = localScale;
        UpdateTransform();
    }


    glm::vec3 GetWorldPos() const { return m_worldPos; }

    glm::mat4 GetLocalMat() const { return m_localMat; }

    glm::mat4 GetWorldMat() const { return m_worldMat; }

    glm::mat3 GetNormalMatrix() const { return m_normMat; }

    glm::vec3 GetRight() const { return m_right; }

    glm::vec3 GetUp() const { return m_up; }

    glm::vec3 GetForward() const { return m_forward; }

    glm::vec3 GetWorldScale() const { return m_worldScale; }


    ModelRenderInfo* GetModelRenderInfo() const;

    const std::vector<std::unique_ptr<IComponent>>& GetComponents() { return m_components; }

    Entity* CopyEntity(std::vector<std::unique_ptr<Entity>>& entities);
};


#endif //GRAFICOS_GAMEOBJECTS_H
