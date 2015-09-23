#include "../mr/vbuffer.hpp"
#include "../mr/buffers/buffer.hpp"

#include "../mr/pre/glew.hpp"

namespace mr {

bool VertexBuffer::_Bind(VertexBuffer* vb, uint32_t binding, uint32_t offset) {
    uint32_t buffer = vb->_vbuf->GetId();
    glBindVertexBuffer(binding, buffer, offset, vb->_vdecl->GetSize());
    VertexDecl::_Bind(vb->_vdecl.get(), binding);
    return true;
}

VertexBufferPtr VertexBuffer::Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl) {
    return VertexBufferPtr(new VertexBuffer(vbuf, vdecl));
}

VertexBuffer::VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl) : _vbuf(vbuf), _vdecl(vdecl) {
}

}
