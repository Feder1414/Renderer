//
// Created by USUARIO on 3/14/2026.
//

#ifndef GRAFICOS_FRAMEBUFFER_H
#define GRAFICOS_FRAMEBUFFER_H
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "RenderBuffer.h"
#include "vec3.hpp"
#include "vec4.hpp"
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

struct FrameBufferCleanValues
{
    glm::vec4 color = glm::vec4(0.5, 0.5f, 0.5f, 1.0f);
    float depthVal = 1.0f;
    int stencilVal = 0;
};

enum class BlitMode
{
    None = 0,
    COLOR = 1 << 0,
    DEPTH = 1 << 1,
    STENCIL = 1 << 2
};


inline BlitMode operator&(BlitMode lo, BlitMode ro)
{
    return static_cast<BlitMode>(static_cast<int>(lo) & static_cast<int>(ro));
}

inline BlitMode operator|(BlitMode lo, BlitMode ro)
{
    return static_cast<BlitMode>(static_cast<int>(lo) | static_cast<int>(ro));
}

inline BlitMode operator~(BlitMode o)
{
    return static_cast<BlitMode>(~static_cast<int>(o));
}

enum class FrameBufferResizeAttachmentsFlags
{
    None = 0,
    COLOR = 1 << 0,
    DEPTH = 1 << 1,
    STENCIL = 1 << 2,
    ALL = 7,
};

inline FrameBufferResizeAttachmentsFlags operator&(FrameBufferResizeAttachmentsFlags lo,
                                                   FrameBufferResizeAttachmentsFlags ro)
{
    return static_cast<FrameBufferResizeAttachmentsFlags>(static_cast<int>(lo) & static_cast<int>(ro));
}

inline FrameBufferResizeAttachmentsFlags operator|(FrameBufferResizeAttachmentsFlags lo,
                                                   FrameBufferResizeAttachmentsFlags ro)
{
    return static_cast<FrameBufferResizeAttachmentsFlags>(static_cast<int>(lo) | static_cast<int>(ro));
}

inline FrameBufferResizeAttachmentsFlags operator~(FrameBufferResizeAttachmentsFlags o)
{
    return static_cast<FrameBufferResizeAttachmentsFlags>(~static_cast<int>(o));
}


enum class BlitFilter
{
    Linear,
    Nearest
};


class FrameBuffer
{
public:
    FrameBuffer(const std::vector<FrameBufferAttachment>& colorAttachments,
                const std::vector<FrameBufferAttachment>& depthStencilAttachments, unsigned int width,
                unsigned int height);
    void ResizeColorAttachment(int channel, int width, int height);
    void ResizeDepthAttachment(int width, int height);
    void ResizeStencilAttachment(int width, int height);
    bool CheckValidColorAttachment(int channel);
    Texture* GetColorAttachmentTexture(int channel);
    Texture* ResolveAttachmentToTexture(const FrameBufferAttachment& attachment) const;
    Texture* GetDepthAttachmentTexture() const;
    void BlitFrameBuffer(FrameBuffer* dst, BlitMode blitMode, BlitFilter blitFilter = BlitFilter::Linear,
                         unsigned int srcX1 = 0, unsigned srcY1 = 0,
                         unsigned int srcX2 = 0, unsigned int srcY2 = 0, unsigned int dstX1 = 0, unsigned dstY1 = 0,
                         unsigned int dstX2 = 0, unsigned int dstY2 = 0);
    std::string GetIdentifierString() const;
    void CleanFrameBuffer(BlitMode attachment, const FrameBufferCleanValues& cleanVal = {}, int channel = 0);
    BlitMode CheckBlitCorrectness(BlitMode blitMode, std::string action);
    void ResizeAttachments(int width, int height);
    void BindFrameBuffer() const;
    void CheckAttachmentCorrectSize(const Attachment& attachment);

    void SetDrawColorAttachment(int index);
    unsigned int GetCurrDrawColorAttachment() const;
    bool CheckColorAttachmentWithinRange(int channel) const;
    void SetReadColorAttachment(int index) const;

    void SetName(const std::string& name) { m_name = name; }

    unsigned int GetFrameBufferId() const { return m_frameBuffer; }

    unsigned int GetWidth() const { return m_width; }
    unsigned int GetHeight() const { return m_height; }

private:
    //DepthStencilAttachment is supposed to contain both depth and stencil or only depth attachment;
    std::vector<FrameBufferAttachment> m_colorAttachment = {};
    std::string m_name;
    FrameBufferAttachment m_depthAttachment;
    FrameBufferAttachment m_stencilAttachment;
    bool m_combinedDepthAndStencilAttachment = false;
    bool m_hasAloneDepthAttach = false;
    bool m_hasAloneStencilAttach = false;
    unsigned int m_width = 1;
    unsigned int m_height = 1;
    unsigned int m_frameBuffer;
    unsigned int m_currDrawAttachment = 0;


    void BindAttachment(Attachment& frameBufferAttachment,
                        GLenum typeAttachment);

    void VisitorResizeAttachment(Attachment& attachment, int width, int height);
};


#endif //GRAFICOS_FRAMEBUFFER_H
