#include "mr/vbuffer.hpp"
#include "mr/buffer/buffer.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

bool VertexBuffer::Bind(uint32_t binding, uint32_t offset) {
    uint32_t buffer = _vbuf->GetId();
    glBindVertexBuffer(binding, buffer, offset, _vdecl->GetSize());
    return _vdecl->Bind();
}

VertexBufferPtr VertexBuffer::Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) {
    return VertexBufferPtr(new VertexBuffer(vbuf, vdecl, num));
}

VertexBuffer::VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) : _vbuf(vbuf), _vdecl(vdecl), _num(num) {
}

}