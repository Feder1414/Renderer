//
// Created by USUARIO on 3/27/2026.
//

#include "ViewPort.h"
#include "glad/glad.h"

void ViewPort::SetSize(unsigned int width, unsigned int height)
{
    m_width = width;
    m_height = height;
}

void ViewPort::SetSizeFrameBuffer(int width, int height)
{
    m_frameBuffer->ResizeAttachments(width, height);
}

void ViewPort::SetDimensions(float top, float left, float width, float height)
{
    m_top = top;
    m_Left = left;
    m_width = width;
    m_height = height;
}

void ViewPort::BindViewPort() const
{
    auto frameBufferWidth = m_frameBuffer->GetWidth();
    auto frameBufferHeight = m_frameBuffer->GetHeight();
    glViewport(m_top * frameBufferWidth, m_Left * frameBufferHeight, m_widthF * frameBufferWidth,
               m_heightF * frameBufferHeight);
}


