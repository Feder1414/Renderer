//
// Created by USUARIO on 12/23/2025.
//

#include "Mesh.h"

#include <iostream>

#include "Material.h"
#include "VertexLayout.h"
#include "Rendering/Buffer.h"


namespace
{
    unsigned int bindingVboIndexInstancing = 2;
}

void Mesh::CreateBuffers(std::unique_ptr<Buffer>& vao, std::unordered_map<BindingIndex, std::unique_ptr<Buffer>>& vbos,
                         std::unique_ptr<Buffer>& ebo,
                         VertexLayout* vertexLayout, std::vector<float>& vertexData)
{
    BufferDesc eboDesc = {
        .name = "ebo mesh: " + m_meshKey, .type = BufferType::Index, .storage = BufferStorage::None,
        .size = m_IndexData.size() * sizeof(float)
    };
    ebo = std::make_unique<Buffer>();
    ebo->CreateBufferRaw(eboDesc, m_IndexData.data());


    //Separate the data that goes in every Buffer
    const auto& vbosAttribs = vertexLayout->GetVboAttribs();
    auto& linearVBOAttribs = vertexLayout->GetLinearVboAttribs();
    const auto& attribNameToLinVertexAttrib = vertexLayout->GetVertexNameToLinearVertexAttrib();
    std::unordered_map<BindingIndex, std::vector<float>> bindingIndexVboToVertexData = {};

    for (std::size_t i = 0; i < vertexData.size(); i += linearVBOAttribs.amountComponents)
    {
        for (const auto& [vboBindingIndex, vboAttribs] : vbosAttribs)
        {
            if (vboAttribs.isPerInstance)
            {
                continue;
            }

            if (!bindingIndexVboToVertexData.contains(vboBindingIndex))
            {
                bindingIndexVboToVertexData[vboBindingIndex] = {};
            }

            for (const auto& attrib : vboAttribs.vertexAttribs)
            {
                auto linearAttribLoc = attribNameToLinVertexAttrib.at(attrib.attributeName);
                for (std::size_t offsetAttrib = i + linearAttribLoc->offsetComponents; offsetAttrib < i +
                     linearAttribLoc->offsetComponents
                     + linearAttribLoc->amountComponents;
                     offsetAttrib++
                )
                {
                    bindingIndexVboToVertexData[vboBindingIndex].push_back(vertexData[offsetAttrib]);
                }
            }
        }
    }


    //Create and fill every VBO
    for (const auto& [bindingIndex, vboAttribs] : vbosAttribs)
    {
        BufferDesc vboDesc;
        vbos[bindingIndex] = std::make_unique<Buffer>();
        if (!vboAttribs.isPerInstance)
        {
            auto& vboVertexData = bindingIndexVboToVertexData.at(bindingIndex);
            vboDesc = {
                .name = "vboMesh " + m_meshKey + "index " + std::to_string(static_cast<int>(bindingIndex)),
                .type = BufferType::Vertex,
                .storage = BufferStorage::None, .size = vboVertexData.size() * sizeof(float)
            };
            vbos[bindingIndex]->CreateBufferRaw(vboDesc, vboVertexData.data());
        }
        else
        {
            vboDesc = {
                .name = "vboMesh " + m_meshKey + "index " + std::to_string(static_cast<int>(bindingIndex)),
                .type = BufferType::Instance,
                .storage = BufferStorage::DynamicStorage,
                .size = m_instanceBuffer.capacity * m_instanceBuffer.instanceSize
            };
            vbos[bindingIndex]->CreateBufferRaw(vboDesc, nullptr);
        }
    }

    //CreateVao
    vao = std::make_unique<Buffer>();
    vao->CreateVao(vertexLayout, vbos, ebo.get());
}


void Mesh::ConvertVertexDataToTargetLayout()
{
    if (!m_targetLayout)
    {
        std::cout << "There is no target layout, vertexData is not gonna be converted" << std::endl;
        return;
    }

    const auto& originalVertexNameToLinearAttrib = m_vertexLayout->GetVertexNameToLinearAttrib();

    const auto& linearVboAttribs = m_vertexLayout->GetLinearVboAttribs();

    const auto& targetLinearVboAttribs = m_targetLayout->GetLinearVboAttribs();

    auto& vertexNameToLinearAttrib = m_vertexLayout->GetVertexNameToLinearVertexAttrib();


    for (unsigned int vertexDataIndex = 0; vertexDataIndex < m_VertexData.size(); vertexDataIndex +=
         linearVboAttribs.amountComponents)
    {
        for (const auto& targetAttrib : targetLinearVboAttribs.vertexAttribs)
        {
            if (vertexNameToLinearAttrib.contains(targetAttrib.attributeName))
            {
                //auto originalVertAttribIndex = vertexNameToIndex[vertexAttrib.attributeName];
                auto& originalVertexAttrib = originalVertexNameToLinearAttrib.at(targetAttrib.attributeName);

                auto attribStartPos = vertexDataIndex + originalVertexAttrib->offsetComponents;
                auto attribEndPos = vertexDataIndex + originalVertexAttrib->offsetComponents +
                    originalVertexAttrib->amountComponents;
                for (unsigned int vertexComponentIndex = attribStartPos; vertexComponentIndex < attribEndPos;
                     vertexComponentIndex++)
                {
                    m_VertexDataConverted.push_back(m_VertexData[vertexComponentIndex]);
                }
            }
            else
            {
                //If the vertex attrib of the target layout is not in the original fill with 0.0f
                for (int component = 0; component < targetAttrib.amountComponents; component++)
                {
                    m_VertexDataConverted.push_back(0.0f);
                }
            }
        }
    }


    m_useConvertedVertexData = true;
    m_bindingIndexToVboConverted.clear();

    CreateBuffers(m_vaoConverted, m_bindingIndexToVboConverted, m_eboConverted, m_targetLayout.get(),
                  m_VertexDataConverted);


    std::cout << "vertex data succefully converted" << std::endl;
}


void Mesh::SetShader(Shader* shader)
{
    for (auto& material : m_submeshToMaterial)
    {
        material->SetShader(shader);
    }
}


const VertexLayout* Mesh::GetVertexLayout() const
{
    if (m_usingInstancing)
    {
        return m_instancingLayout.get();
    }
    if (m_useConvertedVertexData && m_targetLayout)
    {
        return m_targetLayout.get();
    }

    return m_vertexLayout.get();
}

unsigned int Mesh::GetVao() const
{
    if (m_usingInstancing)
    {
        return m_vaoInstancing->GetBufferId();
    }
    if (m_useConvertedVertexData && m_targetLayout)
    {
        return m_vaoConverted->GetBufferId();
    }

    return m_vao->GetBufferId();
}

void Mesh::SetInstancing(const InstanceBuffer& instance_buffer, bool useParamBuffer)
{
    if (useParamBuffer)
    {
        m_instanceBuffer = instance_buffer;
    }

    auto isUsingTargetLayout = (m_useConvertedVertexData && m_targetLayout);
    auto& m_currVertexLayout = isUsingTargetLayout ? m_targetLayout : m_vertexLayout;

    auto& vertexData = isUsingTargetLayout ? m_VertexDataConverted : m_VertexData;

    auto newVertexLayout = m_currVertexLayout->GetOriginalVerticesAttribs();
    for (int i = 0; i < 4; i++)
    {
        newVertexLayout.push_back({
            .type = VertexAttributeType::FLOAT,
            .attributeName = VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes::TRANSFORM, i),
            .amountComponents = 4,
            .perInstance = true,
            .vbo = BindingIndex::Instance

        });
    }


    m_instancingLayout = std::make_unique<VertexLayout>(newVertexLayout);


    m_usingInstancing = true;


    CreateBuffers(m_vaoInstancing, m_bindingIndexToVboInstancing, m_ebo, m_instancingLayout.get(), vertexData);
}

void Mesh::SetInstancingOff()
{
    m_usingInstancing = false;
}


void Mesh::SetInstancesTransform(const void* data, size_t size)
{
    if (!m_usingInstancing)
    {
        return;
    }
    auto vboPerInstance = m_bindingIndexToVboInstancing[BindingIndex::Instance].get();


    bool needsReallocateInstance;
    vboPerInstance->BufferUploadData(data, 0, size, needsReallocateInstance);
    if (needsReallocateInstance)
    {
        auto instanceBufferStride = m_instancingLayout->GetVboAttribs().at(BindingIndex::Instance).accumulatedSize.
                                                        back();
        auto extraRealloc = 1;
        auto factorRealloc = 1000;
        vboPerInstance->ResizeWithHelper(m_instanceBuffer.capacity * m_instanceBuffer.instanceSize * factorRealloc,
                                         size, extraRealloc, data);
        m_vaoInstancing->ReassignVBOToVAO(vboPerInstance, BindingIndex::Instance, 0, instanceBufferStride);
    }
}
