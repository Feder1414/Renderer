//
// Created by USUARIO on 12/23/2025.
//

#include "Mesh.h"

#include <iostream>

#include "Material.h"
#include "VertexLayout.h"


void Mesh::CreateBuffers(unsigned int& vao, unsigned int& vbo, unsigned int& ebo,
                         std::shared_ptr<VertexLayout>& vertexLayout, std::vector<float>& vertexData)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(),
                 GL_STATIC_DRAW);


    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_IndexData.size() * sizeof(unsigned int), m_IndexData.data(),
                 GL_STATIC_DRAW);

    vertexLayout->SetAttribsVao(vao);
    glBindVertexArray(0);
}

void Mesh::ConvertVertexDataToTargetLayout()
{
    if (!m_targetLayout)
    {
        std::cout << "There is no target layout, vertexData is not gonna be converted" << std::endl;
        return;
    }

    auto originalVertexAttribs = m_vertexLayout->GetVertexAttribute();

    auto targetVertexAttribs = m_targetLayout->GetVertexAttribute();

    std::unordered_map<std::string, unsigned int> vertexNameToIndex = {};

    for (unsigned int i = 0; i < originalVertexAttribs.size(); i++)
    {
        auto& vertexAttrib = originalVertexAttribs[i];
        vertexNameToIndex[vertexAttrib.attributeName] = i;
    }


    for (unsigned int vertexDataIndex = 0; vertexDataIndex < m_VertexData.size(); vertexDataIndex += m_vertexLayout->
         GetComponentsPerVertex())
    {
        for (auto& vertexAttrib : targetVertexAttribs)
        {
            if (vertexNameToIndex.contains(vertexAttrib.attributeName))
            {
                auto originalVertAttribIndex = vertexNameToIndex[vertexAttrib.attributeName];
                auto& originalVertexAttrib = originalVertexAttribs[originalVertAttribIndex];

                auto attribStartPos = vertexDataIndex + originalVertexAttrib.offsetComponents;
                auto attribEndPos = vertexDataIndex + originalVertexAttrib.offsetComponents + originalVertexAttrib.
                    amountComponents;
                for (unsigned int vertexComponentIndex = attribStartPos; vertexComponentIndex < attribEndPos;
                     vertexComponentIndex++)
                {
                    m_VertexDataConverted.push_back(m_VertexData[vertexComponentIndex]);
                }
            }
            else
            {
                //If the vertex attrib of the target layout is not in the original fill with 0.0f
                for (int component = 0; component < vertexAttrib.amountComponents; component++)
                {
                    m_VertexDataConverted.push_back(0.0f);
                }
            }
        }
    }
    m_useConvertedVertexData = true;
    CreateBuffers(m_vaoConverted, m_vboConverted, m_eboConverted, m_targetLayout, m_VertexDataConverted);


    std::cout << "vertex data succefully converted" << std::endl;
}


void Mesh::SetShader(Shader* shader)
{
    for (auto& material : m_submeshToMaterial)
    {
        material->SetShader(shader);
    }
}


Buffers Mesh::GetBuffers() const
{
    if (m_useConvertedVertexData && m_targetLayout)
    {
        return {.vao = m_vaoConverted, .vbo = m_vboConverted, .ebo = m_eboConverted};
    }
    return {.vao = m_vao, .vbo = m_vbo, .ebo = m_ebo};
}
