#pragma once

#include "build.hpp"

#include <memory>
#include <future>

namespace mr {

typedef std::shared_ptr<class VertexBuffer> VertexBufferPtr;
typedef std::shared_ptr<class IndexBuffer> IndexBufferPtr;
typedef std::shared_ptr<class ShaderProgram> ShaderProgramPtr;

enum class DrawMode : uint32_t {
    Point = 0x0000,
    Line = 0x0001,
    LineLoop = 0x0002,
    LineStrip = 0x0003,
    Triangle = 0x0004,
    TriangleStrip = 0x0005,
    TriangleFan = 0x0006,
    Quads = 0x0007,
    QuadStrip = 0x0008,
    Polygon = 0x0009
};

enum class ClearFlags : uint32_t {
    Color = 0x00004000,
    Depth = 0x00000100,
    Accum = 0x00000200,
    Stencil = 0x00000400,
    ColorDepth = ClearFlags::Color | ClearFlags::Depth,
    ColorDepthStencil = ClearFlags::ColorDepth | ClearFlags::Stencil,
    All = ClearFlags::ColorDepthStencil | ClearFlags::Accum
};

class MR_API Draw final {
public:
    // Clear current framebuffer
    static bool Clear(ClearFlags const& flags);

    static bool SetClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
};

}
