//
// Created by USUARIO on 12/24/2025.
//

#include "ModelRenderInfo.h"
#include "OpenGL.h"


ModelRenderInfo::ModelRenderInfo(Mesh* model, const VertexLayout& vertexLayout, Material* material) : m_mesh(model),
    m_material(material)
{
    CreateBuffers(vertexLayout);
}

void ModelRenderInfo::CreateBuffers(const VertexLayout& vertexLayout)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, m_mesh->GetVertexData().size() * sizeof(float), m_mesh->GetVertexData().data(),
                 GL_STATIC_DRAW);


    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mesh->GetIndexData().size() * sizeof(int), m_mesh->GetIndexData().data(),
                 GL_STATIC_DRAW);

    vertexLayout.SetAttribsVao(m_vao);
}

void ModelRenderInfo::SetEntity(Entity* entity)
{
    m_entity = entity;
}

ModelBuffers ModelRenderInfo::GetBufferInfo() const
{
    return {
        .vao = m_vao, .vbo = m_vertexBuffer, .ebo = m_indexBuffer,
        .amountVertex = static_cast<int>(m_mesh->GetVertexData().size()),
        .amountIndex = static_cast<int>(m_mesh->GetIndexData().size())

    };
}

