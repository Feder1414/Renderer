//
// Created by USUARIO on 3/14/2026.
//

#ifndef GRAFICOS_FRAMEBUFFER_H
#define GRAFICOS_FRAMEBUFFER_H
#include <memory>
#include <optional>
#include <variant>
#include <vector>

#include "RenderBuffer.h"
#include "glad/glad.h"
class Texture;
class RenderBuffer;

using Attachment = std::variant<std::monostate, std::shared_ptr<RenderBuffer>, std::shared_ptr<Texture>>;

// enum class FrameBufferReadWriteMode
// {
//     Read,
//     Write,
//     Read
// };


enum class FrameBufferTypeAttachment
{
    Color,
    Depth,
    Stencil,
    DepthStencil,
};

struct FrameBufferAttachment
{
    Attachment attachment;
    FrameBufferTypeAttachment typeAttachment;
};

class FrameBuffer
{
public:
    FrameBuffer(const std::vector<FrameBufferAttachment>& colorAttachments,
                const std::vector<FrameBufferAttachment>& depthStencilAttachments);
    void ResizeColorAttachment(int channel, int width, int height);
    void ResizeDepthAttachment(int width, int height);
    void ResizeStencilAttachment(int width, int height);
    bool CheckValidColorAttachment(int channel);
    Texture* GetColorAttachmentTexture(int channel);
    void BindFrameBuffer() const;

private:
    //DepthStencilAttachment is supposed to contain both depth and stencil or only depth attachment;
    std::vector<FrameBufferAttachment> m_colorAttachment = {};
    FrameBufferAttachment m_depthAttachment;
    FrameBufferAttachment m_stencilAttachment;
    bool m_combinedDepthAndStencilAttachment = false;
    bool m_hasAloneDepthAttach = false;
    bool m_hasAloneStencilAttach = false;
    unsigned int m_frameBuffer;

    void BindAttachment(Attachment& frameBufferAttachment,
                        GLenum typeAttachment);


    void VisitorResizeAttachment(Attachment& attachment, int width, int height);
};


#endif //GRAFICOS_FRAMEBUFFER_H
