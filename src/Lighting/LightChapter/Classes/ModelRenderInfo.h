//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_MODELRENDERINFO_H
#define GRAFICOS_MODELRENDERINFO_H
#include <iostream>

#include "IComponent.h"
#include "Material.h"
#include "Mesh.h"
#include "Transform.h"
#include "VertexLayout.h"

class Entity;

class ModelRenderInfo: public  IComponent;

enum class ObjectType
{
    Static,
    Dynamic
};

struct ModelBuffers
{
    unsigned int vao, vbo, ebo;
    int amountVertex, amountIndex;
};

struct sModelRenderInfo
{
    ModelBuffers buffers;
    bool renderMany = false;
};

class ModelRenderInfo : public IComponent
{
private:
    ObjectType m_objectType = ObjectType::Dynamic;
    std::shared_ptr<Mesh> m_mesh;
    std::vector<shared_ptr<Mesh>>
    std::shared_ptr<Material> m_material;


    unsigned int m_vao, m_vertexBuffer, m_indexBuffer;


    std::unordered_map<std::string, UniformValue> uniformValuesInstances;

public:
    const std::unordered_map<std::string, UniformValue>& GetUniformValuesInstances() const
    {
        return uniformValuesInstances;
    }

    ModelRenderInfo(Mesh* model, const VertexLayout& vertexLayout, Material* material);
    void CreateBuffers(const VertexLayout& vertexLayout);

    ModelBuffers GetBufferInfo() const;


    Material* GetMaterial() const { return m_material.get(); }


    void SetDynamic() { m_objectType = ObjectType::Dynamic; }
    void SetStatic() { m_objectType = ObjectType::Static; }
};


#endif //GRAFICOS_MODELRENDERINFO_H
