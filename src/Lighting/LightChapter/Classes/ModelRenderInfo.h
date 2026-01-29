//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_MODELRENDERINFO_H
#define GRAFICOS_MODELRENDERINFO_H
#include <iostream>
#include "Material.h"
#include "Model.h"
#include "Transform.h"
#include "VertexLayout.h"

class Entity;

class ModelRenderInfo;

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

class ModelRenderInfo
{
private:
    ObjectType m_objectType = ObjectType::Dynamic;
    Model* m_model;
    Material* m_material;
    Entity* m_entity;


    unsigned int m_vao, m_vertexBuffer, m_indexBuffer;


    std::unordered_map<std::string, UniformValue> uniformValuesInstances;

public:
    const std::unordered_map<std::string, UniformValue>& GetUniformValuesInstances() const
    {
        return uniformValuesInstances;
    }


    ModelRenderInfo(Model* model, const VertexLayout& vertexLayout, Material* material);
    void CreateBuffers(const VertexLayout& vertexLayout);

    ModelBuffers GetBufferInfo() const;


    Material* GetMaterial() const { return m_material; }

    void SetEntity(Entity* entity);
    const Entity* GetEntity() const { return m_entity; }

    void SetDynamic() { m_objectType = ObjectType::Dynamic; }
    void SetStatic() { m_objectType = ObjectType::Static; }
};


#endif //GRAFICOS_MODELRENDERINFO_H
