//
// Created by USUARIO on 3/14/2026.
//

#include "FrameBuffer.h"

#include <format>
#include <iostream>
#include "Texture.h"
#include "glad/glad.h"
#include "EnumUtils.h"
#include "gtc/type_ptr.inl"


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

    GLbitfield BlitModeToGl(BlitMode blitMode)
    {
        GLbitfield flags = 0;

        if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::COLOR)) != static_cast<int>(BlitMode::None))
        {
            flags |= GL_COLOR_BUFFER_BIT;
        }
        if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::DEPTH)) != static_cast<int>(BlitMode::None))
        {
            flags |= GL_DEPTH_BUFFER_BIT;
        }
        if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::STENCIL)) != static_cast<int>(BlitMode::None))
        {
            flags |= GL_STENCIL_BUFFER_BIT;
        }
        return flags;
    }

    GLenum BlitFilterToGlFilter(BlitFilter blitFilter)
    {
        switch (blitFilter)
        {
        case BlitFilter::Linear: return GL_LINEAR;
        case BlitFilter::Nearest: return GL_NEAREST;
        default: throw std::exception("Not valid Blit Filter enum");
        }
    }
}

FrameBuffer::FrameBuffer(const std::vector<FrameBufferAttachment>& colorAttachments,
                         const std::vector<FrameBufferAttachment>& depthStencilAttachments, unsigned int width,
                         unsigned int height)
{
    m_width = width;
    m_height = height;
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
            CheckAttachmentCorrectSize(m_depthAttachment.attachment);
        }
        else if (frameBufferAttachment.typeAttachment == FrameBufferTypeAttachment::Stencil)
        {
            m_stencilAttachment = frameBufferAttachment;
            amountStencilBuffers += 1;
            m_hasAloneStencilAttach = true;
            CheckAttachmentCorrectSize(m_stencilAttachment.attachment);
        }
        else if (frameBufferAttachment.typeAttachment == FrameBufferTypeAttachment::DepthStencil)
        {
            m_depthAttachment = frameBufferAttachment;
            m_stencilAttachment = frameBufferAttachment;
            amountStencilBuffers += 1;
            amountDepthBuffers += 1;
            m_combinedDepthAndStencilAttachment = true;
            CheckAttachmentCorrectSize(m_depthAttachment.attachment);
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


    for (int i = 0; i < m_colorAttachment.size(); i++)
    {
        auto glAttachmentType = FrameBufferTypeAttachmentToGlAttachment(FrameBufferTypeAttachment::Color, i);
        CheckAttachmentCorrectSize(m_colorAttachment[i].attachment);
        BindAttachment(m_colorAttachment[i].attachment, glAttachmentType);
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
        std::cerr << "The framebuffer with id: " << std::to_string(m_frameBuffer) << "  name" << m_name <<
            " is not completed, the status is "
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

void FrameBuffer::CheckAttachmentCorrectSize(const Attachment& attachment)
{
    std::visit([this](auto&& x)
    {
        using T = std::decay_t<decltype(x)>;
        unsigned int attachmentWidth;
        unsigned int attachmentHeight;
        if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
        {
            const auto& texDesc = x->GetTextureDesc();

            attachmentWidth = texDesc.width;
            attachmentHeight = texDesc.height;
        }
        else if constexpr (std::is_same_v<T, std::shared_ptr<RenderBuffer>>)
        {
            const auto& renderBufferDesc = x->GetDesc();

            attachmentWidth = renderBufferDesc.width;
            attachmentHeight = renderBufferDesc.height;
        }

        if (this->m_width != attachmentWidth || this->m_height != attachmentHeight)
        {
            std::string errMessage = std::format("{} does not have all the attachments of the same dimensions",
                                                 this->GetIdentifierString());
            std::cerr << errMessage << std::endl;
            throw std::logic_error(errMessage);
        }
    }, attachment);
}

void FrameBuffer::SetDrawColorAttachment(int channel)
{
    if (!CheckColorAttachmentWithinRange(channel))
    {
        return;
    }
    if (channel == -1)
    {
        glNamedFramebufferDrawBuffer(m_frameBuffer, GL_NONE);
        return;
    }

    m_currDrawAttachment = 0;
    glNamedFramebufferDrawBuffer(m_frameBuffer, GL_COLOR_ATTACHMENT0 + channel);
}

unsigned int FrameBuffer::GetCurrDrawColorAttachment() const
{
    return m_currDrawAttachment;
}


void FrameBuffer::SetReadColorAttachment(int channel) const
{
    if (!CheckColorAttachmentWithinRange(channel))
    {
        return;
    }
    if (channel == -1)
    {
        glNamedFramebufferReadBuffer(m_frameBuffer, GL_NONE);
        return;
    }

    glNamedFramebufferReadBuffer(m_frameBuffer, GL_COLOR_ATTACHMENT0 + channel);
}

bool FrameBuffer::CheckColorAttachmentWithinRange(int channel) const
{
    if (m_colorAttachment.empty() && channel != -1)
    {
        std::cerr << GetIdentifierString() <<
            "has not color attachments set as draw" << std::endl;
        return false;
    }

    if (channel < -1)
    {
        std::cerr << "The channel index must be greather or equal than -1 " << std::endl;
        return false;
    }

    if (channel >= m_colorAttachment.size())
    {
        std::cerr << "The curr channel of " << channel << "is outside of the max channel " << m_colorAttachment.size() -
            1 << std::endl;
        return false;
    }
    return true;
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
        std::cout << GetIdentifierString() << "there are no depth attachments to resize" << std::endl;
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
        std::cout << GetIdentifierString() << "does not have stencil attachments to resize" << std::endl;;
    }
}

bool FrameBuffer::CheckValidColorAttachment(int channel)
{
    if (m_colorAttachment.empty())
    {
        std::cout << GetIdentifierString() << "Has no color attachments to resize" << std::endl;
        return false;
    }
    if (channel >= m_colorAttachment.size())
    {
        std::cout << GetIdentifierString() << "The channel: " << std::to_string(channel) <<
            " is out of the range of the color attachments" <<
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
    return ResolveAttachmentToTexture(m_colorAttachment[channel]);
}

Texture* FrameBuffer::ResolveAttachmentToTexture(const FrameBufferAttachment& attachment) const
{
    Texture* texture = nullptr;
    std::visit([&texture](auto&& x)
    {
        using T = std::decay_t<decltype(x)>;
        if constexpr (std::is_same_v<T, std::shared_ptr<Texture>>)
        {
            texture = x.get();
        }
    }, attachment.attachment);

    return texture;
}

Texture* FrameBuffer::GetDepthAttachmentTexture() const
{
    if (!m_hasAloneDepthAttach && !m_combinedDepthAndStencilAttachment)
    {
        std::cerr << GetIdentifierString() << " does not have Depth attachment" << std::endl;
        return nullptr;
    }

    return ResolveAttachmentToTexture(m_depthAttachment);
}

void FrameBuffer::BlitFrameBuffer(FrameBuffer* dst, BlitMode blitMode, BlitFilter blitFilter, unsigned int srcX0,
                                  unsigned srcY0,
                                  unsigned int srcX1, unsigned int srcY1, unsigned int dstX0, unsigned dstY0,
                                  unsigned int dstX1, unsigned int dstY1)
{
    auto correctBlitMode = CheckBlitCorrectness(blitMode, "blit");
    auto glBlitMode = BlitModeToGl(correctBlitMode);
    glBlitNamedFramebuffer(m_frameBuffer, dst->m_frameBuffer, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1,
                           glBlitMode, BlitFilterToGlFilter(blitFilter));
}

std::string FrameBuffer::GetIdentifierString() const
{
    return std::format("The framebuffer with id {} and name {} ", m_frameBuffer, m_name);
}


void FrameBuffer::CleanFrameBuffer(BlitMode attachment, const FrameBufferCleanValues& cleanVal,
                                   int channel)
{
    auto correctBlitMode = CheckBlitCorrectness(attachment, "clean");

    if ((correctBlitMode & BlitMode::COLOR) != BlitMode::None)
    {
        if (!CheckColorAttachmentWithinRange(channel))
        {
            std::cout << GetIdentifierString() << "cannot clean to frameBuffer out of range" << std::endl;
            return;
        }
        glClearNamedFramebufferfv(m_frameBuffer, GL_COLOR, channel, glm::value_ptr(cleanVal.color));
    }

    if (((correctBlitMode & BlitMode::DEPTH) != BlitMode::None) && ((correctBlitMode & BlitMode::STENCIL) !=
        BlitMode::None) && m_combinedDepthAndStencilAttachment)
    {
        glClearNamedFramebufferfi(m_frameBuffer, GL_DEPTH_STENCIL, 0, cleanVal.depthVal, cleanVal.stencilVal);
    }
    else
    {
        if ((correctBlitMode & BlitMode::STENCIL) != BlitMode::None)
        {
            glClearNamedFramebufferiv(m_frameBuffer, GL_STENCIL, 0, &cleanVal.stencilVal);
        }
        if ((correctBlitMode & BlitMode::DEPTH) != BlitMode::None)
        {
            glClearNamedFramebufferfv(m_frameBuffer, GL_DEPTH, 0, &cleanVal.depthVal);
        }
    }
}

BlitMode FrameBuffer::CheckBlitCorrectness(BlitMode blitMode, std::string action)
{
    BlitMode correctBlitMode = blitMode;
    if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::COLOR)) && m_colorAttachment.empty())
    {
        std::cerr << std::format("{} does not have a color attachment to {}", GetIdentifierString(), action) <<
            std::endl;
        correctBlitMode = correctBlitMode & ~BlitMode::COLOR;
    }
    if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::DEPTH)) && !(m_hasAloneDepthAttach |
        m_combinedDepthAndStencilAttachment))
    {
        std::cerr << std::format("{} does not have a depth attachment to {}", GetIdentifierString(), action) <<
            std::endl;
        correctBlitMode = correctBlitMode & ~BlitMode::DEPTH;
    }
    if ((static_cast<int>(blitMode) & static_cast<int>(BlitMode::STENCIL)) && !(m_hasAloneStencilAttach |
        m_combinedDepthAndStencilAttachment))
    {
        std::cerr << std::format("{} does not have a stencil attachment to {}", GetIdentifierString(), action) <<
            std::endl;
        correctBlitMode = correctBlitMode & ~BlitMode::STENCIL;
    }
    return correctBlitMode;
}

void FrameBuffer::ResizeAttachments(int width, int height)
{
    m_width = width;
    m_height = height;
    if (m_combinedDepthAndStencilAttachment)
    {
        ResizeDepthAttachment(width, height);
    }

    if (m_hasAloneDepthAttach)
    {
        ResizeDepthAttachment(width, height);
    }
    if (m_hasAloneStencilAttach)
    {
        ResizeStencilAttachment(width, height);
    }

    for (int i = 0; i < m_colorAttachment.size(); i++)
    {
        ResizeColorAttachment(i, width, height);
    }
}
