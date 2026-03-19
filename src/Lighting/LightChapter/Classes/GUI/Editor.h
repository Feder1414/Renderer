//
// Created by USUARIO on 2/8/2026.
//

#ifndef GRAFICOS_EDITOR_H
#define GRAFICOS_EDITOR_H

#include "ComponentInspector.h"
#include "IWidget.h"


#include "EntityTree.h"
#include "GeneralRendererSettingsComponent.h"

class Engine;

class Editor : public IWidget
{
public:
    void Render() override;
    void OnEntitySelectedEntityTree(Entity* entity);
    std::unique_ptr<EntityTree> m_entityTree;
    std::unique_ptr<ComponentInspector> m_componentInspector;
    std::unique_ptr<GeneralRendererSettingsComponent> m_generalRenderSettings;
    Engine* m_engine;

    explicit Editor(Engine* engine);
};


#endif //GRAFICOS_EDITOR_H
