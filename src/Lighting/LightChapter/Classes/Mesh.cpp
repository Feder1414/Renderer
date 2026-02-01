//
// Created by USUARIO on 12/23/2025.
//

#include "Mesh.h"
#include "VertexLayout.h"


void Mesh::CreateBuffers(const VertexLayout& vertexLayout)
{
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_VertexData.size() * sizeof(float), m_VertexData.data(),
                 GL_STATIC_DRAW);


    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexData.size() * sizeof(int), m_IndexData.data(),
                 GL_STATIC_DRAW);

    vertexLayout.SetAttribsVao(m_vao);
}
