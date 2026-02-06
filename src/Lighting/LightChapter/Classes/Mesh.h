//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_MODEL_H
#define GRAFICOS_MODEL_H
#include <memory>
#include <vector>
#include <glad/glad.h>

#include "VertexLayout.h"


class Shader;
class Material;

struct Buffers
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;
};


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
    bool m_useConvertedVertexData = false;

    unsigned int m_vao, m_vbo, m_ebo;

    unsigned int m_vaoConverted, m_vboConverted, m_eboConverted;
    std::vector<float> m_VertexData = {};
    std::vector<unsigned int> m_IndexData = {};

    std::shared_ptr<VertexLayout> m_vertexLayout;

    std::shared_ptr<VertexLayout> m_targetLayout;
    std::vector<float> m_VertexDataConverted;

    std::vector<SubMesh> m_Submeshes;

    std::vector<std::shared_ptr<Material>> m_submeshToMaterial;
    void ConvertVertexDataToTargetLayout();

public:
    Mesh(const std::vector<float>& vertexData, const std::vector<unsigned int>& indexData,

         std::shared_ptr<VertexLayout>& vertexLayout, std::vector<SubMesh>& submesh,
         std::vector<std::shared_ptr<Material>>& submeshToMaterial)
    {
        m_VertexData = vertexData;
        m_IndexData = indexData;
        m_vertexLayout = vertexLayout;
        m_Submeshes = submesh;
        m_submeshToMaterial = submeshToMaterial;
        CreateBuffers(m_vao, m_vbo, m_ebo, m_vertexLayout, m_VertexData);
    }

    void SetTargetVertexLayout(const std::shared_ptr<VertexLayout>& vertexLayout)
    {
        m_targetLayout = vertexLayout;
        ConvertVertexDataToTargetLayout();
    }


    std::vector<float>& GetVertexData()
    {
        if (m_useConvertedVertexData && m_targetLayout)
        {
            return m_VertexDataConverted;
        }
        return m_VertexData;
    }

    std::vector<unsigned int> GetIndexData()
    {
        return m_IndexData;
    }

    const std::vector<SubMesh>& GetSubMeshes() { return m_Submeshes; }
    const std::vector<std::shared_ptr<Material>>& GetSubmeshToMaterial() { return m_submeshToMaterial; }

    Buffers GetBuffers() const;

    void SetShader(Shader*);


    void CreateBuffers(unsigned int& vao, unsigned int& vbo, unsigned int& ebo,
                       std::shared_ptr<VertexLayout>& vertexLayout, std::vector<float>& vertexData);
};


#endif //GRAFICOS_MODEL_H
