//
// Created by USUARIO on 2/8/2026.
//

#ifndef GRAFICOS_EDITOR_H
#define GRAFICOS_EDITOR_H

#include "ComponentInspector.h"
#include "IWidget.h"


#include "EntityTree.h"
#include "GeneralRendererSettingsComponent.h"
#include "SceneView.h"


class Engine;

class Editor : public IWidget
{
public:
    void Render() override;
    void OnEntitySelectedEntityTree(Entity* entity);
    std::unique_ptr<EntityTree> m_entityTree;
    std::unique_ptr<ComponentInspector> m_componentInspector;
    std::unique_ptr<GeneralRendererSettingsComponent> m_generalRenderSettings;
    std::unique_ptr<SceneView> m_sceneView;
    Engine* m_engine;
    bool m_initialized = false;

    explicit Editor(Engine* engine);
};


#endif //GRAFICOS_EDITOR_H
