//
// Created by USUARIO on 3/14/2026.
//

#include "FrameBuffer.h"

#include <iostream>
#include "Texture.h"
#include "glad/glad.h"


namespace
{
    GLenum FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment typeAttachment, int index = 0)
    {
        switch (typeAttachment)
        {
        case FrameBufferTypeAttachment::Color: return GL_COLOR_ATTACHMENT0 + index;
        case FrameBufferTypeAttachment::Depth: return GL_DEPTH_ATTACHMENT;
        case FrameBufferTypeAttachment::Stencil: return GL_STENCIL_ATTACHMENT;
        case FrameBufferTypeAttachment::DepthStencil: return GL_DEPTH_STENCIL_ATTACHMENT;
        }
    }
}

FrameBuffer::FrameBuffer(const std::vector<FrameBufferAttachment>& colorAttachments,
                         const std::vector<FrameBufferAttachment>& depthStencilAttachments)
{
    if (colorAttachments.empty() && depthStencilAttachments.empty())
    {
        std::cerr << "Framebuffers must have at least one attachment" << std::endl;
        throw std::exception("Framebuffers must have at least one attachment");
    }

    int amountDepthBuffers = 0;
    int amountStencilBuffers = 0;

    m_colorAttachment = colorAttachments;

    for (const auto& frameBufferAttachment : depthStencilAttachments)
    {
        if (frameBufferAttachment.typeAttachment == FrameBufferTypeAttachment::Depth)
        {
            m_depthAttachment = frameBufferAttachment;
            amountDepthBuffers += 1;
            m_hasAloneDepthAttach = true;
        }
        else if (frameBufferAttachment.typeAttachment == FrameBufferTypeAttachment::Stencil)
        {
            m_stencilAttachment = frameBufferAttachment;
            amountStencilBuffers += 1;
            m_hasAloneStencilAttach = true;
        }
        else if (frameBufferAttachment.typeAttachment == FrameBufferTypeAttachment::DepthStencil)
        {
            m_depthAttachment = frameBufferAttachment;
            m_stencilAttachment = frameBufferAttachment;
            amountStencilBuffers += 1;
            amountDepthBuffers += 1;
            m_combinedDepthAndStencilAttachment = true;
        }

        if (amountDepthBuffers > 1)
        {
            std::cerr << "Render buffer can only have one depth buffer" << std::endl;
            throw std::exception("Render buffer can only have one depth buffer");
        }
        if (amountStencilBuffers > 1)
        {
            std::cerr << "Render buffer can only have one depth buffer" << std::endl;
            throw std::exception("Render buffer can only have one depth buffer");
        }
    }

    glCreateFramebuffers(1, &m_frameBuffer);


    if (!m_colorAttachment.empty())
    {
        for (int i = 0; i < m_colorAttachment.size(); i++)
        {
            auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::Color, i);
            BindAttachment(m_colorAttachment[i].attachment, glAttachmentType);
        }
    }

    if (m_combinedDepthAndStencilAttachment)
    {
        auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::DepthStencil);
        BindAttachment(m_depthAttachment.attachment, glAttachmentType);
    }
    else if (m_hasAloneDepthAttach)
    {
        auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::Depth);
        BindAttachment(m_depthAttachment.attachment, glAttachmentType);
    }
    else if (m_hasAloneStencilAttach)
    {
        auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::Stencil);
        BindAttachment(m_stencilAttachment.attachment, glAttachmentType);
    }

    auto status = glCheckNamedFramebufferStatus(m_frameBuffer, GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "The framebuffer with id: " << std::to_string(m_frameBuffer) << " is not completed, the status is "
            << status << std::endl;
        throw std::exception("Incomplete framebuffer");
    }
}


void FrameBuffer::BindAttachment(Attachment& frameBufferAttachment,
                                 GLenum typeAttachmentGl)
{
    std::visit([typeAttachmentGl, this](auto&& x)
    {
        using T = std::decay_t<decltype(x)>;

        if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
        {
            auto textureId = x->GetTextureId();

            glNamedFramebufferTexture(m_frameBuffer, typeAttachmentGl, textureId, 0);
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<RenderBuffer>>)
        {
            auto renderBufferId = x->GetRenderBufferId();
            glNamedFramebufferRenderbuffer(m_frameBuffer, typeAttachmentGl, GL_RENDERBUFFER, renderBufferId);
        }
    }, frameBufferAttachment);
}

void FrameBuffer::BindFrameBuffer() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
}

void FrameBuffer::ResizeColorAttachment(int channel, int width, int height)
{
    if (!CheckValidColorAttachment(channel))
    {
        return;
    }
    VisitorResizeAttachment(m_colorAttachment[channel].attachment, width, height);

    auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::Color, channel);
    BindAttachment(m_colorAttachment[channel].attachment, glAttachmentType);
}

void FrameBuffer::VisitorResizeAttachment(Attachment& attachment, int width, int height)
{
    std::visit([width, height](auto&& x)
    {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
        {
            x->ResizeTexture(width, height);
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<RenderBuffer>>)
        {
            x->ResizeRenderBuffer(width, height);
        }
    }, attachment);
}

void FrameBuffer::ResizeDepthAttachment(int width, int height)
{
    if (m_combinedDepthAndStencilAttachment || m_hasAloneDepthAttach)
    {
        VisitorResizeAttachment(m_depthAttachment.attachment, width, height);
        auto glTypeAttachment = FrameBufferTypeAttachmentToGlAttachment(m_depthAttachment.typeAttachment);
        BindAttachment(m_depthAttachment.attachment, glTypeAttachment);
    }
    else
    {
        std::cout << "There are no depth attachments in the frame buffer " << std::to_string(m_frameBuffer) <<
            " to resize" << std::endl;;
    }
}

void FrameBuffer::ResizeStencilAttachment(int width, int height)
{
    if (m_combinedDepthAndStencilAttachment)
    {
        VisitorResizeAttachment(m_depthAttachment.attachment, width, height);
        auto glTypeAttachment = FrameBufferTypeAttachmentToGlAttachment(m_depthAttachment.typeAttachment);
        BindAttachment(m_depthAttachment.attachment, glTypeAttachment);
    }
    else if (m_hasAloneStencilAttach)
    {
        VisitorResizeAttachment(m_stencilAttachment.attachment, width, height);
        auto glTypeAttachment = FrameBufferTypeAttachmentToGlAttachment(m_stencilAttachment.typeAttachment);
        BindAttachment(m_stencilAttachment.attachment, glTypeAttachment);
    }
    else
    {
        std::cout << "There are no stencil attachments in the frame buffer " << std::to_string(m_frameBuffer) <<
            " to resize" << std::endl;;
    }
}

bool FrameBuffer::CheckValidColorAttachment(int channel)
{
    if (m_colorAttachment.empty())
    {
        std::cout << "There are no color attachments in the frame buffer " << std::to_string(m_frameBuffer) <<
            "  resize";
        return false;
    }
    if (channel >= m_colorAttachment.size())
    {
        std::cout << "The channel: " << std::to_string(channel) << " is out of the range of the color attachments" <<
            std::endl;
        return false;
    }
    return true;
}

Texture* FrameBuffer::GetColorAttachmentTexture(int channel)
{
    if (!CheckValidColorAttachment(channel))
    {
        return nullptr;
    }
    Texture* texture = nullptr;
    std::visit([&texture](auto&& x)
    {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
        {
            texture = x.get();
        }
    }, m_colorAttachment[channel].attachment);
    return texture;
}
