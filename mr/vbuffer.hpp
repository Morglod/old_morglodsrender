#pragma once

#include "build.hpp"
#include "vformat.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class VertexBuffer> VertexBufferPtr;

class MR_API VertexBuffer final {
    friend class Primitive;
    friend class _Alloc;
public:
    bool Bind(uint32_t binding, uint32_t offset = 0);
    static VertexBufferPtr Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);

    inline BufferPtr GetBuffer() const;
    inline VertexDeclPtr GetVertexDecl() const;
    inline uint32_t GetNum() const;
protected:
    VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);

private:
    BufferPtr _vbuf = nullptr;
    VertexDeclPtr _vdecl = nullptr;
    uint32_t _num = 0;
};

inline BufferPtr VertexBuffer::GetBuffer() const {
    return _vbuf;
}

inline VertexDeclPtr VertexBuffer::GetVertexDecl() const {
    return _vdecl;
}

inline uint32_t VertexBuffer::GetNum() const {
    return _num;
}

}
