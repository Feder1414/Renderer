//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_MODEL_H
#define GRAFICOS_MODEL_H
#include <memory>
#include <vector>
#include <glad/glad.h>

#include "VertexLayout.h"


struct SubMesh
{
    //Offset from indexData
    unsigned int indexOffset;
    unsigned int indexCount;

    unsigned int vertexOffset;
    unsigned int vertexCount;
};

class Mesh
{
private:
    unsigned int m_vao, m_vbo, m_ebo;
    std::vector<float> m_VertexData = {};
    std::vector<int> m_IndexData = {};

    std::shared_ptr<VertexLayout> m_vertexLayout;
    std::shared_ptr<VertexLayout> m_targetLayout;
    std::vector<int> m_VertexDataConverted;

    std::vector<SubMesh> m_Submeshes;

public:
    Mesh(const std::vector<float>& vertexData, const std::vector<int>& indexData,
         std::shared_ptr<VertexLayout>& vertexLayout)
    {
        m_VertexData = vertexData;
        m_IndexData = indexData;
        m_vertexLayout = vertexLayout;
    }

    std::vector<float>& GetVertexData()
    {
        return m_VertexData;
    }

    std::vector<int>& GetIndexData()
    {
        return m_IndexData;
    }


    void CreateBuffers(const VertexLayout& vertexLayout);
};


#endif //GRAFICOS_MODEL_H
