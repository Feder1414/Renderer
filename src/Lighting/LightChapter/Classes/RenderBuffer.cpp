//
// Created by USUARIO on 3/15/2026.
//

#include "RenderBuffer.h"

#include "glad/glad.h"


namespace
{
    GLenum RenderBufferIntFormatToGlFormat(RenderBufferInternalFormat intFormat)
    {
        switch (intFormat)
        {
        case RenderBufferInternalFormat::DEPTH_COMPONENT_16: return GL_DEPTH_COMPONENT16;
        case RenderBufferInternalFormat::DEPTH_COMPONENT_24: return GL_DEPTH_COMPONENT24;
        case RenderBufferInternalFormat::DEPTH_COMPONENT_32: return GL_DEPTH_COMPONENT32;
        case RenderBufferInternalFormat::DEPTH24_STENCIL_8: return GL_DEPTH24_STENCIL8;
        case RenderBufferInternalFormat::STENCIL_8: return GL_STENCIL_INDEX8;
        }
    };
}

RenderBuffer::RenderBuffer(const RenderBufferDesc& renderBufferDesc)
{
    m_renderBufferDesc = renderBufferDesc;
    glCreateRenderbuffers(1, &m_renderBuffer);
    auto intFormat = RenderBufferIntFormatToGlFormat(m_renderBufferDesc.intFormat);

    if (m_renderBufferDesc.renderBufferType == RenderBufferType::Normal)
    {
        glNamedRenderbufferStorage(m_renderBuffer, intFormat, m_renderBufferDesc.width, m_renderBufferDesc.height);
    }
    else if (m_renderBufferDesc.renderBufferType == RenderBufferType::Multisampled)
    {
        glNamedRenderbufferStorageMultisample(m_renderBuffer, m_renderBufferDesc.amountSamples, intFormat,
                                              m_renderBufferDesc.width, m_renderBufferDesc.height);
    }
}

void RenderBuffer::ResizeRenderBuffer(int width, int height)
{
    m_renderBufferDesc.width = width;
    m_renderBufferDesc.height = height;
    auto intFormat = RenderBufferIntFormatToGlFormat(m_renderBufferDesc.intFormat);
    if (m_renderBufferDesc.renderBufferType == RenderBufferType::Normal)
    {
        glNamedRenderbufferStorage(m_renderBuffer, intFormat, m_renderBufferDesc.width, m_renderBufferDesc.height);
    }
    else if (m_renderBufferDesc.renderBufferType == RenderBufferType::Multisampled)
    {
        glNamedRenderbufferStorageMultisample(m_renderBuffer, m_renderBufferDesc.amountSamples, intFormat,
                                              m_renderBufferDesc.width, m_renderBufferDesc.height);
    }
}
