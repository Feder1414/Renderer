//
// Created by USUARIO on 2/8/2026.
//

#ifndef GRAFICOS_ENTITYTREE_H
#define GRAFICOS_ENTITYTREE_H
#include <string>

#include "Entity.h"
#include "Event.h"
#include "IWidget.h"
class Scene;

class EntityTree : public IWidget
{
public:

    EntityTree(Scene* scene) : IWidget("Tree panel") { m_scene = scene; }
    void Render() override;
    Event<Entity*> onEntitySelected;

private:

    Scene* m_scene;
    Entity* m_focusedEntity;
    std::string m_panelId = "treePanel";


    void DrawNodeEntity(Entity* entity);
};


#endif //GRAFICOS_ENTITYTREE_H
