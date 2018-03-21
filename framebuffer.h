#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <cctype>
#include <tuple>
#include <memory>
//#include "model.h"
#include "core.h"
using Color24 = QVector3D;
using Color32 = QVector4D;


///TODO::this class can be simplified by template traits
class FrameBuffer
{
public:
    static constexpr int BUFFER_DEPTH = 3;
public:
    FrameBuffer()noexcept : m_width(0), m_height(0) {}
    FrameBuffer(std::size_t width, std::size_t height)noexcept:
        m_width(width),
        m_height(height)
    {
        resize(width,height);
    }
    const unsigned char * buffer()const { return m_buffer.get(); }
    std::size_t height()const { return m_height; }
    std::size_t width()const { return m_width; }
    void resize(int width,int height);

    void setColor24(const Point2i & p, const Color24 & color24);
    void setColor32(const Point2i & p, const Color32 & color32);

    void setColor24(const Color24 & color24);
    void setColor32(const Color32 & color32);
    void setColor24(int x, int y, const Color24 & color24);
    void setColor32(int x, int y, const Color32 & color32);
    void setColor24(int index, const Color24 & color24);
    void setColor32(int index, const Color32 & color32);

    void setHorizontialLineColor24(int y, int startx, int endx, const Color24& color24);
private:
    std::size_t m_width;
    std::size_t m_height;

    std::unique_ptr<unsigned char[]> m_buffer;
};

#endif // FRAMEBUFFER_H
