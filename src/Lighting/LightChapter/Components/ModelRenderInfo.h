//
// Created by USUARIO on 12/24/2025.
//

#ifndef GRAFICOS_MODELRENDERINFO_H
#define GRAFICOS_MODELRENDERINFO_H


#include "IComponent.h"
#include "../Classes/Material.h"
#include "../Classes/Mesh.h"


class Entity;


enum class ObjectType
{
    Static,
    Dynamic
};

struct ModelBuffers
{
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

    //Maps the submesh [i] ot the m_mesh to the material that uses


    std::unordered_map<std::string, UniformValue> uniformValuesInstances;

public:

    std::unique_ptr<IComponent> Clone();


    const std::unordered_map<std::string, UniformValue>& GetUniformValuesInstances() const
    {
        return uniformValuesInstances;
    }

    ModelRenderInfo(std::shared_ptr<Mesh>& model);


    //Material* GetMaterial() const { return m_material.get(); }

    const std::string& GetComponentName() override
    {
        static std::string name = "RenderInfo";
        return name;
    };
    void SetDynamic() { m_objectType = ObjectType::Dynamic; }
    void SetStatic() { m_objectType = ObjectType::Static; }
    void SetShader(Shader* shader) { m_mesh->SetShader(shader); }

    void Update() override
    {
    };

    Mesh* GetMesh() const { return m_mesh.get(); }

    std::shared_ptr<Mesh> GetMeshShared() const { return m_mesh; }




    void SetShaderSubmeshes(Shader* shader) { m_mesh->SetShader(shader); }


    void SetComponentMetadata() override;
};


#endif //GRAFICOS_MODELRENDERINFO_H
