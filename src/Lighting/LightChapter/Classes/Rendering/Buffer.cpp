//
// Created by USUARIO on 3/19/2026.
//

#include "Buffer.h"

#include <iostream>
#include <memory>

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

    GLbitfield BufferStorageToGlStorage(BufferStorage bufferStorage)
    {
        GLbitfield flags = 0;
        if ((bufferStorage & BufferStorage::DynamicStorage) != BufferStorage::None)
        {
            flags |= GL_DYNAMIC_STORAGE_BIT;
        }
        if ((bufferStorage & BufferStorage::MapRead) != BufferStorage::None)
        {
            flags |= GL_MAP_READ_BIT;
        }
        if ((bufferStorage & BufferStorage::MapWrite) != BufferStorage::None)
        {
            flags |= GL_MAP_WRITE_BIT;
        }
        if ((bufferStorage & BufferStorage::MapPersistent) != BufferStorage::None)
        {
            flags |= GL_MAP_PERSISTENT_BIT;
        }
        if ((bufferStorage & BufferStorage::MapCoherent) != BufferStorage::None)
        {
            flags |= GL_MAP_COHERENT_BIT;
        }
        if ((bufferStorage & BufferStorage::MapCoherent) != BufferStorage::None)
        {
            flags |= GL_CLIENT_STORAGE_BIT;
        }

        return flags;
    }
}

void Buffer::CreateVao(VertexLayout* vertexLayout,
                       const std::unordered_map<BindingIndex, std::unique_ptr<Buffer>>& bindingIndexToBufferObject,
                       Buffer* ebo)
{
    const auto& vbosAttribs = vertexLayout->GetVboAttribs();
    glCreateVertexArrays(1, &m_buffer);

    for (const auto& [vboBindingIndex, vboAttribs] : vbosAttribs)
    {
        auto vbo = bindingIndexToBufferObject.at(vboBindingIndex).get();

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
    glVertexArrayElementBuffer(m_buffer, ebo->GetBufferId());
}


void Buffer::CreateBufferRaw(const BufferDesc& bufferDesc, const void* data)
{
    m_BufferDesc = bufferDesc;
    CreateBufferFromBuffDesc(data);
}

void Buffer::ReassignVBOToVAO(const Buffer* buffer, BindingIndex bindingIndex, size_t offset, size_t stride)
{
    if (m_BufferDesc.type != BufferType::VAO)
    {
        std::cout << "Cannot reassign VBO to a buffer that is not a VAO" << std::endl;
        return;
    }

    glVertexArrayVertexBuffer(
        m_buffer,
        static_cast<int>(bindingIndex),
        buffer->GetBufferId(),
        offset,
        stride
    );
}

void Buffer::CreateBufferFromBuffDesc(const void* data)
{
    glCreateBuffers(1, &m_buffer);
    glNamedBufferStorage(m_buffer, m_BufferDesc.size, data, BufferStorageToGlStorage(m_BufferDesc.storage));
}

void Buffer::ResizeWithHelper(size_t factorRealloc, size_t totalSize, size_t extraRealloc, const void* data)
{
    if (m_BufferDesc.size >= totalSize)
    {
        return;
    }

    auto diffSize = totalSize - m_BufferDesc.size;
    auto timesFactorRealloc = diffSize / factorRealloc;
    auto extraReallocMemorySize = factorRealloc * (timesFactorRealloc + extraRealloc);


    auto newSize = m_BufferDesc.size + extraReallocMemorySize;
    Resize(newSize, data);
}


void Buffer::Resize(size_t newSize, const void* data)
{
    if (m_BufferDesc.type == BufferType::VAO)
    {
        std::cerr << "VAO buffers cannot be resized" << std::endl;
        return;
    }
    glDeleteBuffers(1, &m_buffer);
    m_BufferDesc.size = newSize;

    CreateBufferFromBuffDesc(nullptr);
}


void Buffer::BufferUploadData(const void* data, size_t offset, size_t size, bool& needsReallocate)
{
    needsReallocate = false;
    if ((m_BufferDesc.storage & BufferStorage::DynamicStorage) == BufferStorage::None)
    {
        std::cout << "The buffer with name: " << m_BufferDesc.name << "is inmutable and cannot be modified" <<
            std::endl;
        return;
    }

    if (size + offset > m_BufferDesc.size)
    {
        needsReallocate = true;
        std::cout << "The buffer with name: " << m_BufferDesc.name <<
            "does not have enough memory allocated for the incoming data" << std::endl;
        return;
    }
    glNamedBufferSubData(m_buffer, offset, size, data);
}
