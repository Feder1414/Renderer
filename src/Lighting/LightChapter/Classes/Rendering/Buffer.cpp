//
// Created by USUARIO on 3/19/2026.
//

#include "Buffer.h"
#include "VertexLayout.h"


namespace
{
    GLenum BufferUsageToGlUsage(BufferUsage bufferUsage)
    {
        switch (bufferUsage)
        {
        case BufferUsage::DynamicDraw: return GL_DYNAMIC_DRAW;
        case BufferUsage::StaticDraw: return GL_STATIC_DRAW;
        default: throw std::exception("Invalid buffer usage");
        }
    };
}

void Buffer::CreateVao(VertexLayout* vertexLayout,
                       const std::unordered_map<BindingIndex, Buffer*>& bindingIndexToBufferObject)
{
    const auto& vbosAttribs = vertexLayout->GetVboAttribs();
    glCreateVertexArrays(1, &m_buffer);

    for (const auto& [vboBindingIndex, vboAttribs] : vbosAttribs)
    {
        auto vbo = bindingIndexToBufferObject.at(vboBindingIndex);

        glVertexArrayVertexBuffer(
            m_buffer,
            static_cast<int>(vboBindingIndex),
            vbo->GetBufferId(),
            0,
            vboAttribs.accumulatedSize[vboAttribs.accumulatedSize.size() - 1]
        );
        for (const auto& attrib : vboAttribs.vertexAttribs)
        {
            glEnableVertexArrayAttrib(m_buffer, attrib.location);
            glVertexArrayAttribFormat(m_buffer, attrib.location, attrib.amountComponents,
                                      attrib.glType, GL_FALSE,
                                      static_cast<GLuint>(attrib.offset));
            glVertexArrayAttribBinding(m_buffer, attrib.location, static_cast<int>(vboBindingIndex));

            if (vboAttribs.isPerInstance)
            {
                glVertexArrayBindingDivisor(m_buffer, static_cast<int>(vboBindingIndex), 1);
            }
        }
    }
}


void Buffer::CreateBufferRaw(const BufferDesc& bufferDesc, const void* data)
{
    m_BufferDesc = bufferDesc;
    glCreateBuffers(1, &m_buffer);
    glNamedBufferData(m_buffer, m_BufferDesc.size, data, BufferUsageToGlUsage(m_BufferDesc.usage));
}


void Buffer::BufferUploadData(const void* data, size_t offset, size_t size)
{
    glNamedBufferSubData(m_buffer, offset, size, data);
}
