//
// Created by USUARIO on 12/23/2025.
//

#include "Mesh.h"

#include <iostream>

#include "Material.h"
#include "VertexLayout.h"


namespace
{
    unsigned int bindingVboIndexInstancing = 2;
}

void Mesh::CreateBuffers(unsigned int& vao, std::unordered_map<BindingIndex, unsigned int>& vbos, unsigned int& ebo,
                         VertexLayout* vertexLayout, std::vector<float>& vertexData)
{
    const auto& vbosAttribs = vertexLayout->GetVboAttribs();


    glCreateVertexArrays(1, &vao);
    // Corregido

    for (const auto& [vboBindingIndex, vboAttribs] : vbosAttribs)
    {
        unsigned int newVbo;
        glCreateBuffers(1, &newVbo);
        vbos[vboBindingIndex] = newVbo;

        glVertexArrayVertexBuffer(
            vao,
            static_cast<int>(vboBindingIndex),
            newVbo,
            0,
            vboAttribs.accumulatedSize[vboAttribs.accumulatedSize.size() - 1]
        );
        for (const auto& attrib : vboAttribs.vertexAttribs)
        {
            glEnableVertexArrayAttrib(vao, attrib.location);
            glVertexArrayAttribFormat(vao, attrib.location, attrib.amountComponents,
                                      attrib.glType, GL_FALSE,
                                      static_cast<GLuint>(attrib.offset));
            glVertexArrayAttribBinding(vao, attrib.location, static_cast<int>(vboBindingIndex));

            if (vboAttribs.isPerInstance)
            {
                glVertexArrayBindingDivisor(vao, static_cast<int>(vboBindingIndex), 1);
            }
        }
    }


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

    for (const auto& [bindingIndex, vboAttribs] : vbosAttribs)
    {
        auto vbo = vbos[bindingIndex];
        if (!vboAttribs.isPerInstance)
        {
            auto& vboVertexData = bindingIndexVboToVertexData.at(bindingIndex);
            glNamedBufferData(vbo, vboVertexData.size() * sizeof(float), vboVertexData.data(), GL_STATIC_DRAW);
        }
        else
        {
            glNamedBufferData(vbo, vboAttribs.vertexAttribs[0].stride * m_instanceBuffer.capacity, nullptr,
                              GL_DYNAMIC_DRAW);
        }
    }


    glCreateBuffers(1, &ebo);
    glNamedBufferData(ebo, m_IndexData.size() * sizeof(unsigned int), m_IndexData.data(), GL_STATIC_DRAW);

    glVertexArrayElementBuffer(vao, ebo);
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


Buffers Mesh::GetBuffers() const
{
    if (m_useConvertedVertexData && m_targetLayout)
    {
        return {.vao = m_vaoConverted, .vbo = m_vboConverted, .ebo = m_eboConverted};
    }

    return {.vao = m_vao, .vbo = m_vbo, .ebo = m_ebo};
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
        return m_vaoInstancing;
    }
    if (m_useConvertedVertexData && m_targetLayout)
    {
        return m_vaoConverted;
    }

    return m_vao;
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
    m_vboInstancing = {};

    CreateBuffers(m_vaoInstancing, m_bindingIndexToVboInstancing, m_ebo, m_instancingLayout.get(), vertexData);
}

void Mesh::SetInstancingOff()
{
    m_usingInstancing = false;
}


void Mesh::SetInstancesTransform(const std::vector<glm::mat4>& transforms)
{
    auto vboPerInstance = m_bindingIndexToVboInstancing[BindingIndex::Instance];

    const auto& vboPerInstanceAttribs = m_instancingLayout->GetVboAttribs().at(BindingIndex::Instance);

    //Asumes is tighly packaged
    auto bytesSizePerInstance = vboPerInstanceAttribs.vertexAttribs[0].stride;


    if (transforms.size() > m_instanceBuffer.capacity)
    {
        auto capacityDiff = transforms.size() - m_instanceBuffer.capacity;
        auto factorIncrease = (capacityDiff / m_instanceBuffer.factorRealloc) + 2;
        auto sumCapacity = factorIncrease * m_instanceBuffer.factorRealloc;
        m_instanceBuffer.capacity += sumCapacity;

        glNamedBufferData(vboPerInstance, bytesSizePerInstance * m_instanceBuffer.capacity, nullptr,
                          GL_DYNAMIC_DRAW);
    }

    // for (unsigned int i = 0; i < transforms.size(); i++)
    // {
    //     auto offsetBytes = bytesSizePerInstance * i;
    //
    //     glNamedBufferSubData(vboPerInstance, offsetBytes, sizeof(transforms[i]), glm::value_ptr(transforms[i]));
    // }
    glNamedBufferSubData(vboPerInstance, 0, sizeof(glm::mat4) * transforms.size(), transforms.data());
}
