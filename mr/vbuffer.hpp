#pragma once

#include "build.hpp"
#include "vformat.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;
typedef std::shared_ptr<class VertexBuffer> VertexBufferPtr;

class MR_API VertexBuffer final {
    friend class Draw;
public:
    inline uint32_t GetNum() const;
    std::future<bool> Bind(uint32_t binding, uint32_t offset = 0);
    static VertexBufferPtr Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);
    static bool _Bind(VertexBuffer* vb, uint32_t binding, uint32_t offset);
    uint32_t _num;
protected:
    VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);
private:
    BufferPtr _vbuf = nullptr;
    VertexDeclPtr _vdecl = nullptr;
};

inline uint32_t VertexBuffer::GetNum() const {
    return _num;
}

}
