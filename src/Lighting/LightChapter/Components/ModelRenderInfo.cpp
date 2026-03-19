//
// Created by USUARIO on 12/24/2025.
//

#include "ModelRenderInfo.h"
#include "OpenGL.h"


ModelRenderInfo::ModelRenderInfo(std::shared_ptr<Mesh>& m_model) : m_mesh(m_model)
{
}

void ModelRenderInfo::SetComponentMetadata()
{
}

std::unique_ptr<IComponent> ModelRenderInfo::Clone()
{
    auto cloneRenderInfo = std::make_unique<ModelRenderInfo>(m_mesh);
    cloneRenderInfo->m_objectType = m_objectType;
    cloneRenderInfo->uniformValuesInstances = uniformValuesInstances;
    return std::move(cloneRenderInfo);
}
