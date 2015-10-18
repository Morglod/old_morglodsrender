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

class MR_API Draw final {
public:
    static bool Clear(uint32_t flags);
    static bool ClearColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static bool Primitive(ShaderProgramPtr const& program, DrawMode const& dmode, VertexBufferPtr const& vb, IndexBufferPtr const& ib = nullptr);
};

}
