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
    inline uint32_t GetNum() const { return _num; }
    std::future<bool> Bind(uint32_t binding, uint32_t offset = 0);
    static VertexBufferPtr Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);
protected:
    VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num);
    static bool _Bind(VertexBuffer* vb, uint32_t binding, uint32_t offset);
private:
    BufferPtr _vbuf = nullptr;
    VertexDeclPtr _vdecl = nullptr;
    uint32_t _num;
};

}
