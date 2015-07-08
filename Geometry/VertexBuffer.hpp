/**

VertexBuffer provides interface to work with verticies on gpu.

Example:

struct Vertex {
    vec3 pos;
    vec2 tex;
};

VertexFormatPtr format = ...; //describes format of Vertex struct
const int num = 10;
Vertex verticies[num]; //geometry data

VertexBufferPtr vb = VertexBuffer::Create(format, num);
Vertex* vertexData = vb->MapVertex<Vertex>();
for(int i = 0; i < num; ++i) vertexData[i] = verticies[i];
vb->UnMap();

**/

#pragma once

#include "../Buffers/BuffersInterfaces.hpp"
#include "GeometryFormats.hpp"

#include <memory>

namespace mr {

class VertexBuffer;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

class VertexBuffer {
public:
    static VertexBufferPtr Create(VertexFormatPtr const& format, size_t const& vertexesNum, BufferUsage const& usage = BufferUsage::Static);
    static VertexBufferPtr Create(VertexFormatPtr const& format, IBuffer* buffer, size_t const& vertexesNum, size_t const& offset = 0);
    static VertexBufferPtr Create(VertexFormatPtr const& format, IBufferRangeHdlWPtr const& range);

    inline VertexFormatPtr GetFormat() const { return _format; }
    inline IBufferRangeHdlPtr GetBufferRange() const { return _range; }
    size_t CalcVertexCount() const;

    virtual bool Write(void* data, size_t const& dataSize, size_t const& offset = 0);

    virtual char* Map(int const& flags = IBuffer::IMappedRange::Write);
    virtual char* Map(size_t const& offset, size_t const& length, int const& flags = IBuffer::IMappedRange::Write);
    virtual void UnMap();
    inline IBuffer::IMappedRangePtr GetMapping() const { return _mapping; }

    template<typename TVertex>
    TVertex* MapVertex(int const& flags = IBuffer::IMappedRange::Write) {
        char* mem = Map(flags);
        if(mem == nullptr) return nullptr;
        return (TVertex*)(mem);
    }

    template<typename TVertex>
    TVertex* MapVertex(size_t const& vertexNumOffset, size_t const& vertexNum, int const& flags = IBuffer::IMappedRange::Write) {
        char* mem = Map(vertexNumOffset * _format->size, vertexNum * _format->size, flags);
        if(mem == nullptr) return nullptr;
        return (TVertex*)(mem);
    }

    virtual void Destroy();

    virtual ~VertexBuffer();
protected:
    VertexBuffer(VertexFormatPtr const& format, IBufferRangeHdlPtr const& range, bool ownBuffer);

    IBufferRangeHdlPtr _range = nullptr;
    VertexFormatPtr _format = nullptr;
    IBuffer::IMappedRangePtr _mapping = nullptr;
    bool _ownBuffer = false;
};

}
