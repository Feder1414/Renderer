//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_MODEL_H
#define GRAFICOS_MODEL_H
#include <vector>
#include <glad/glad.h>


class Model
{
private:
    std::vector<float> m_VertexData = {};
    std::vector<int> m_IndexData = {};

public:
    Model(const std::vector<float>& vertexData, const std::vector<int>& indexData)
    {
        m_VertexData = vertexData;
        m_IndexData = indexData;
    }

    std::vector<float>& GetVertexData()
    {
        return m_VertexData;
    }

    std::vector<int>& GetIndexData()
    {
        return m_IndexData;
    }
};


#endif //GRAFICOS_MODEL_H
