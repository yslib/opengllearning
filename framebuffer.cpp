#include "framebuffer.h"

void FrameBuffer::resize(int width, int height)
{
    m_buffer.reset(new unsigned char[width*height*BUFFER_DEPTH]);
    m_width = width;
    m_height = height;
}

void FrameBuffer::setColor24(const Point2i& p, const Color24 & color24)
{
    setColor24(p.x(), p.y(), color24);
}

void FrameBuffer::setColor32(const Point2i& p, const Color32 & color32)
{
    setColor32(p.x(), p.y(), color32);
}

void FrameBuffer::setColor24(const Color24 &color24)
{
    for (int i = 0; i < m_height; i++) {
        for (int j = 0; j < m_width; j++) {
            int index = j + i * m_width;
            m_buffer[3*index] = color24[0];
            m_buffer[3*index + 1] = color24[1];
            m_buffer[3*index + 2] = color24[2];
        }
    }
}

void FrameBuffer::setColor32(const Color32 &color32)
{
    for (int i = 0; i < m_height; i++) {
        for (int j = 0; j < m_width; j++) {
            int index = j + i * m_width;
            m_buffer[4*index] = color32[0];
            m_buffer[4*index + 1] = color32[1];
            m_buffer[4*index + 2] = color32[2];
            m_buffer[4*index + 3] = color32[3];
        }
    }
}
void FrameBuffer::setColor24(int x, int y, const Color24 &color24)
{
    setColor24(x + y * m_width, color24);
}

void FrameBuffer::setColor32(int x, int y, const Color32 &color32)
{
    setColor32(x + y * m_width, color32);

}

void FrameBuffer::setColor24(int index, const Color24 &color24)
{
    m_buffer[3*index] = color24[0];
    m_buffer[3*index + 1] = color24[1];
    m_buffer[3*index + 2] = color24[2];
}

void FrameBuffer::setColor32(int index, const Color32 &color32)
{
    m_buffer[4*index] = color32[0];
    m_buffer[4*index + 1] = color32[1];
    m_buffer[4*index + 2] =color32[2];
    m_buffer[4*index + 3] = color32[3];
}

void FrameBuffer::setHorizontialLineColor24(int y, int startx, int endx, const Color24 & color24)
{
    for (int x = startx; x < endx; x++) {
        int index = y * m_width + x;
        m_buffer[3 * index] = color24[0];
        m_buffer[3 * index + 1] = color24[1];
        m_buffer[3 * index + 2] = color24[2];
    }
}
