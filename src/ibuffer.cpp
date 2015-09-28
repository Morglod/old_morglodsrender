#include "mr/ibuffer.hpp"
#include "mr/pre/glew.hpp"
#include "mr/buffer/buffer.hpp"

namespace mr {

IndexBufferPtr IndexBuffer::Create(BufferPtr const& ibuf, IndexType const& datatype, uint32_t num) {
}

IndexBufferPtr IndexBuffer::Create(void* array, IndexType const& datatype, uint32_t num) {
}

IndexBuffer::IndexBuffer(BufferPtr const& buf, void* mem, IndexType const& dtype, uint32_t num) : _buf(buf), _mem(mem), _dtype(dtype), _num(num) {
}

bool IndexBuffer::_Bind(IndexBuffer* ib) {
    if(ib->_buf != nullptr) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->_buf->GetId());
    }
    return true;
}

}
