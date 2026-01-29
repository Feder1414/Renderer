//
// Created by USUARIO on 12/24/2025.
//

#include "VertexLayout.h"


VertexLayout::VertexLayout(const std::vector<VertexAttribute>& verticesAttributes)
{
    accumulatedSize = std::vector<int>(verticesAttributes.size());
    for (int i = 0; i < verticesAttributes.size(); i++)
    {
        auto& vertexAttrib = verticesAttributes[i];
        auto locVertexAttrib = vertexAttrib;
        auto vertexAttribTypeInfo = attribTypeToGlType.at(vertexAttrib.type);


        locVertexAttrib.sizeOfType = vertexAttribTypeInfo.size * locVertexAttrib.amountComponents;
        locVertexAttrib.glType = vertexAttribTypeInfo.glType;
        if (i == 0)
        {
            locVertexAttrib.offset = 0;
            accumulatedSize[i] = locVertexAttrib.sizeOfType;
        }
        else
        {
            locVertexAttrib.offset = accumulatedSize[i - 1];
            accumulatedSize[i] = accumulatedSize[i - 1] + locVertexAttrib.sizeOfType;
        }
        m_verticesAttributes.push_back(locVertexAttrib);
    }

    for (auto& vertexAttribute : m_verticesAttributes)
    {
        vertexAttribute.stride = accumulatedSize[accumulatedSize.size() - 1];
    }
}

void VertexLayout::SetAttribsVao(unsigned int vao) const
{
    glBindVertexArray(vao);
    for (int i = 0; i < m_verticesAttributes.size(); i++)
    {
        auto& vertexAttrib = m_verticesAttributes[i];

        glVertexAttribPointer(i, vertexAttrib.amountComponents, vertexAttrib.glType, GL_FALSE, vertexAttrib.stride,
                              (void*)vertexAttrib.offset);
        glEnableVertexAttribArray(i);
    }
}
