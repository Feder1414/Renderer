//
// Created by USUARIO on 12/24/2025.
//

#include "VertexLayout.h"

#include <format>


namespace
{
    std::unordered_map<VertexPredefinedAttributes, std::string> vertexPredAttrToStr = {
        {VertexPredefinedAttributes::POSITION, "position"},
        {VertexPredefinedAttributes::COLOR, "color"},
        {VertexPredefinedAttributes::UV, "uv"},
        {VertexPredefinedAttributes::NORMAL, "normal"},
        {VertexPredefinedAttributes::TANGENT, "tangent"},
        {VertexPredefinedAttributes::BITANGENT, "bitangent"},
        {VertexPredefinedAttributes::BONES, "bones"},
        {VertexPredefinedAttributes::BONES_WEIGHT, "bonesWeight"},
        {VertexPredefinedAttributes::TRANSFORM, "transform"}
    };
}

VertexLayout::VertexLayout(const std::vector<VertexAttribute>& verticesAttributes, bool oneVbo)
{
    m_originalVertexAttribs = verticesAttributes;
    for (unsigned int i = 0; i < verticesAttributes.size(); i++)
    {
        auto& originalVertexAttrib = verticesAttributes[i];
        if (!originalVertexAttrib.perInstance)
        {
            m_linearVboAttribs.vertexAttribs.push_back(originalVertexAttrib);
        }
        if (m_attribNameToLocation.contains(originalVertexAttrib.attributeName))
        {
            auto error = std::format(
                "VertexLayout cannot repeat names for vertex attributes. The vertex layout has the vertex name: {}  repeated ",
                originalVertexAttrib.attributeName);
            throw std::exception(error.c_str());
        }
        m_attribNameToLocation[originalVertexAttrib.attributeName] = i;
    }


    for (auto& vertexAttrib : verticesAttributes)
    {
        if (!m_vboToAttributes.contains(vertexAttrib.vbo))
        {
            m_vboToAttributes[vertexAttrib.vbo] = {};
        }
        m_vboToAttributes[vertexAttrib.vbo].vertexAttribs.push_back(vertexAttrib);
        if (vertexAttrib.perInstance)
        {
            m_vboToAttributes[vertexAttrib.vbo].isPerInstance = true;
        }
        if (m_vboToAttributes[vertexAttrib.vbo].isPerInstance && !vertexAttrib.perInstance)
        {
            std::string error = std::format(
                "Cannot mix attrib with name {} that is not per instance with another attributes that are per isntance in the same VBO",
                vertexAttrib.attributeName);

            throw std::exception(
                error.c_str()
            );
        }
    }

    for (auto& [vbo, vboAttribs] : m_vboToAttributes)
    {
        ProcessVertexAttribsInterleaved(vboAttribs);
    }

    ProcessVertexAttribsInterleaved(m_linearVboAttribs);
    for (unsigned int i = 0; i < m_linearVboAttribs.vertexAttribs.size(); i++)
    {
        auto& vertexAttrib = m_linearVboAttribs.vertexAttribs[i];
        m_vertexNameToLinearVertexAttrib[vertexAttrib.attributeName] = {
            &vertexAttrib

        };
    }
}

void VertexLayout::ProcessVertexAttribsInterleaved(VBOAttribs& vboAttribs)
{
    auto& verticesAttributes = vboAttribs.vertexAttribs;
    vboAttribs.accumulatedSize = std::vector<unsigned int>(verticesAttributes.size());
    auto& accumulatedSize = vboAttribs.accumulatedSize;
    auto offSetComponents = 0;

    auto amountComponentsPerVertex = 0;
    for (int i = 0; i < verticesAttributes.size(); i++)
    {
        auto& vertexAttrib = verticesAttributes[i];
        auto locVertexAttrib = vertexAttrib;
        auto vertexAttribTypeInfo = attribTypeToGlType.at(vertexAttrib.type);

        amountComponentsPerVertex += locVertexAttrib.amountComponents;

        locVertexAttrib.location = m_attribNameToLocation.at(locVertexAttrib.attributeName);


        auto sizeBytesAttribute = vertexAttribTypeInfo.size * locVertexAttrib.amountComponents;
        locVertexAttrib.sizeOfType = sizeBytesAttribute;
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
        locVertexAttrib.offsetComponents = offSetComponents;
        offSetComponents += locVertexAttrib.amountComponents;


        verticesAttributes[i] = locVertexAttrib;
    }
    vboAttribs.amountComponents = amountComponentsPerVertex;

    for (auto& vertexAttribute : verticesAttributes)
    {
        vertexAttribute.stride = accumulatedSize[accumulatedSize.size() - 1];
    }
}


std::string VertexLayout::VertexPredefinedAttrToString(VertexPredefinedAttributes attr, unsigned int channelIndex)
{
    if (attr == VertexPredefinedAttributes::UV || attr == VertexPredefinedAttributes::COLOR || attr ==
        VertexPredefinedAttributes::TRANSFORM)
    {
        return vertexPredAttrToStr[attr] + std::to_string(channelIndex);
    }
    return vertexPredAttrToStr[attr];
}

unsigned int VertexLayout::GetVertexAttribLocation(const std::string& attribName) const
{
    auto attribLocation = m_attribNameToLocation.find(attribName);

    if (attribLocation == m_attribNameToLocation.end())
    {
        return -1;
    }
    return attribLocation->second;
}




//std::size_t VertexLayout::GetBytesPerVertex() { return m_accumulatedSize[m_accumulatedSize.size() - 1]; }
