//
// Created by USUARIO on 12/23/2025.
//

#ifndef GRAFICOS_MODEL_H
#define GRAFICOS_MODEL_H
#include <memory>
#include <vector>
#include <glad/glad.h>

#include "AABB.h"
#include "fwd.hpp"
#include "BV/IBoundingVolume.h"
#include "VertexLayout.h"
#include "Rendering/Buffer.h"
#include "glm.hpp"


class AABB;
class IBoundingVolume;
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

struct InstanceBuffer
{
    size_t capacity = 1000;
    size_t instanceSize = sizeof(glm::mat4);
};


class Mesh
{
private:
    std::string m_meshKey;

    std::unique_ptr<IBoundingVolume> m_boundingVolume = nullptr;
    std::unique_ptr<IBoundingVolume> m_worldBoundingVolume = nullptr;

    bool m_useConvertedVertexData = false;


    std::unique_ptr<Buffer> m_vao, m_ebo = nullptr;
    std::unordered_map<BindingIndex, std::unique_ptr<Buffer>> m_bindingIndexToVbo = {};

    std::unique_ptr<Buffer> m_vaoConverted, m_eboConverted = nullptr;
    std::unordered_map<BindingIndex, std::unique_ptr<Buffer>> m_bindingIndexToVboConverted = {};

    std::unique_ptr<Buffer> m_vaoInstancing, m_eboInstancing = nullptr;
    std::unordered_map<BindingIndex, std::unique_ptr<Buffer>> m_bindingIndexToVboInstancing = {};

    std::vector<float> m_VertexData = {};
    std::vector<unsigned int> m_IndexData = {};

    std::shared_ptr<VertexLayout> m_vertexLayout;

    std::shared_ptr<VertexLayout> m_targetLayout;

    std::unique_ptr<VertexLayout> m_instancingLayout = nullptr;

    AABB* m_aabb;


    std::vector<float> m_VertexDataConverted;

    InstanceBuffer m_instanceBuffer;

    std::vector<SubMesh> m_Submeshes;

    std::vector<std::shared_ptr<Material>> m_submeshToMaterial;

    bool m_usingInstancing = false;
    void ConvertVertexDataToTargetLayout();

public:
    Mesh(const std::string& meshKey, const std::vector<float>& vertexData, const std::vector<unsigned int>& indexData,

         std::shared_ptr<VertexLayout>& vertexLayout, std::vector<SubMesh>& submesh,
         std::vector<std::shared_ptr<Material>>& submeshToMaterial)
    {
        m_VertexData = vertexData;
        m_IndexData = indexData;
        m_vertexLayout = vertexLayout;
        m_Submeshes = submesh;
        m_submeshToMaterial = submeshToMaterial;
        m_meshKey = meshKey;
        CreateBuffers(m_vao, m_bindingIndexToVbo, m_ebo, m_vertexLayout.get(), m_VertexData);
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

    void SetInstancing(const InstanceBuffer& instance_buffer = {}, bool useParamBuffer = false);

    bool GetUsingInstancing()
    {
        return m_usingInstancing;
    }

    void SetInstancesTransform(const void* data, size_t size);

    void SetInstancingOff();

    void SetBoundingVolume(std::unique_ptr<IBoundingVolume> boundingVolume)
    {
        m_boundingVolume = std::move(boundingVolume);
        m_aabb = dynamic_cast<AABB*>(m_boundingVolume.get());
    }


    const VertexLayout* GetVertexLayout() const;
    unsigned int GetVao() const;

    void SetShader(Shader*);

    const std::string& GetMeshKey() { return m_meshKey; }

    IBoundingVolume* GetBoundingVolume() const { return m_boundingVolume.get(); }

    IBoundingVolume* GetWorldBoundingVolume(Entity* entity)
    {
        if (!m_boundingVolume)
        {
            return nullptr;
        }
        m_worldBoundingVolume = std::move(m_boundingVolume->CalculateWorldBB(entity));
        return m_worldBoundingVolume.get();
    }

    IBoundingVolume* GetAABB() const
    {
        return m_aabb;
    }

    void CreateBuffers(std::unique_ptr<Buffer>& vao, std::unordered_map<BindingIndex, std::unique_ptr<Buffer>>& vbos,
                       std::unique_ptr<Buffer>& ebo, VertexLayout* vertexLayout, std::vector<float>& vertexData);
};


#endif //GRAFICOS_MODEL_H
