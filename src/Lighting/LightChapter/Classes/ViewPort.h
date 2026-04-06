//
// Created by USUARIO on 3/27/2026.
//

#ifndef GRAFICOS_VIEWPORT_H
#define GRAFICOS_VIEWPORT_H
#include <iostream>
#include <stdexcept>
#include <variant>

#include "FrameBuffer.h"
#include "Light.h"


class FrameBuffer;
class Camera;

using CamViewPort = std::variant<std::monostate, Camera*, Light*>;


class ViewPort
{
public:
    ViewPort(unsigned int width, unsigned int height, const CamViewPort& camera, FrameBuffer* frameBuffer,
             float top = 0.0f, float left = 0.0f, float widthF = 1.0f, float heightF = 1.0f) :
        m_width(width),
        m_height(height), m_top(top), m_Left(left), m_widthF(widthF), m_heightF(heightF), m_camera(camera),
        m_frameBuffer(frameBuffer)
    {
        if (!frameBuffer)
        {
            throw std::logic_error("Viewports must have a non null framebuffer");
        }
    }

    void SetSize(unsigned int width, unsigned int height);

    void SetSizeFrameBuffer(int width, int height);

    void SetDimensions(float top, float left, float width, float height);

    void BindViewPort() const;

    int GetColorTextureId() const
    {
        auto currChannelColorTex = m_frameBuffer->GetCurrDrawColorAttachment();
        auto currColTex = m_frameBuffer->GetColorAttachmentTexture(currChannelColorTex);

        if (!currColTex)
        {
            std::cerr << "The framebuffer of the viewport does not have a color texture" << std::endl;
            return -1;
        }
        return currColTex->GetTextureId();
    }

private:
    unsigned int m_width = 1, m_height = 1;

    float m_top = 0.0f;
    float m_Left = 0.0f;
    float m_widthF = 1.0f;
    float m_heightF = 1.0f;

    CamViewPort m_camera;

    FrameBuffer* m_frameBuffer = nullptr;

    FrameBufferResizeAttachmentsFlags resizeAttachments = FrameBufferResizeAttachmentsFlags::None;
};


#endif //GRAFICOS_VIEWPORT_H
