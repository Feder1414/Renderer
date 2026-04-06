//
// Created by USUARIO on 3/15/2026.
//

#ifndef GRAFICOS_RENDERBUFFER_H
#define GRAFICOS_RENDERBUFFER_H


enum class RenderBufferInternalFormat
{
    DEPTH_COMPONENT_16,
    DEPTH_COMPONENT_24,
    DEPTH_COMPONENT_32,
    DEPTH24_STENCIL_8,
    STENCIL_8
};

enum class RenderBufferType
{
    Normal,
    Multisampled
};

struct RenderBufferDesc
{
    RenderBufferInternalFormat intFormat;
    RenderBufferType renderBufferType = RenderBufferType::Normal;
    unsigned int amountSamples = 4;
    int width, height;
};

class RenderBuffer
{
public:
    unsigned int GetRenderBufferId() const { return m_renderBuffer; }
    RenderBuffer(const RenderBufferDesc& renderBufferDesc);

    void ResizeRenderBuffer(int width, int height);

    const RenderBufferDesc& GetDesc() const { return m_renderBufferDesc; }

private:
    unsigned int m_renderBuffer;
    RenderBufferDesc m_renderBufferDesc;
};


#endif //GRAFICOS_RENDERBUFFER_H
