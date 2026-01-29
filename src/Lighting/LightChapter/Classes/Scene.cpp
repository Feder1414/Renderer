//
// Created by USUARIO on 1/11/2026.
//

#include "Scene.h"
#include "uuid.h"
#include "UuidGenerator.h"


void Scene::AddEntity(std::unique_ptr<Entity> entity)
{
    const auto uuid = UuidGenerator::GenerateUUID();
    entity->SetId(uuid);
    if (entity->GetComponent<Light>())
    {
        m_lights.push_back(entity.get());
    }
    m_entities.push_back(std::move(entity));
}
