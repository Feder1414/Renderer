//
// Created by USUARIO on 2/9/2026.
//

#ifndef GRAFICOS_COMPONENTINSPECTOR_H
#define GRAFICOS_COMPONENTINSPECTOR_H
#include "../../Components/IComponent.h"
#include "IWidget.h"
#include "vec3.hpp"

class Scene;

enum class TransformUpdated
{
    POSITION,
    ROTATION,
    SCALE,
};

class Entity;

struct PropertyCache
{
    std::any value;
    bool isEditing;
};

class ComponentInspector : public IWidget
{
public:
    ComponentInspector() : IWidget("Component inspector")
    {
    }

    void Render() override;
    void SetEntity(Entity* entity);
    void SetScene(Scene* scene) { m_scene = scene; }
    void RenderGenericComponent(IComponent* component);
    void SetTransformComponent();

protected:

private:
    Entity* m_entity = nullptr;
    Scene* m_scene = nullptr;
    glm::vec3 m_pos = glm::vec3(0.0);
    glm::vec3 m_rot = glm::vec3(0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    std::unordered_map<std::string, std::unordered_map<std::string, std::any>> m_genericComponentValues = {};


    void RenderTransformComponenent();


    void UpdateEntityTransform(TransformUpdated transformUpdated);
};


#endif //GRAFICOS_COMPONENTINSPECTOR_H
