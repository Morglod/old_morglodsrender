#include "mr/ibuffer.hpp"
#include "mr/pre/glew.hpp"
#include "mr/buffer.hpp"
#include "src/statecache.hpp"
#include "src/mp.hpp"
#include "mr/log.hpp"
#include "mr/alloc.hpp"

namespace mr {

IndexBufferPtr IndexBuffer::Create(BufferPtr const& ibuf, IndexDataType const& datatype, uint32_t num) {
    MP_ScopeSample(IndexBuffer::Create);
    if(!ibuf->GetResidentState().resident) {
        MR_LOG_ERROR(IndexBuffer::Create, "Buffer should be resident");
        return nullptr;
    }
    return IndexBufferPtr(MR_NEW(IndexBuffer, ibuf, nullptr, datatype, num));
}

IndexBufferPtr IndexBuffer::Create(MemoryPtr const& mem, IndexDataType const& datatype, uint32_t num) {
    MP_ScopeSample(IndexBuffer::Create);
    return IndexBufferPtr(MR_NEW(IndexBuffer, nullptr, mem, datatype, num));
}

IndexBuffer::IndexBuffer(BufferPtr const& buf, MemoryPtr const& mem, IndexDataType const& dtype, uint32_t num) : _buf(buf), _mem(mem), _dtype(dtype), _num(num) {
}

bool IndexBuffer::Bind() {
    MP_ScopeSample(IndexBuffer::Bind);
    if(!StateCache::Get()->SetIndexBuffer(_buf->GetId())) return true;

    if(_buf != nullptr) {
        if(GLEW_NV_vertex_buffer_unified_memory) {
            glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, _buf->_resident.address, _buf->_size);
        }
        else
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buf->GetId());

    } else
        if(GLEW_NV_vertex_buffer_unified_memory) glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);

    return true;
}

}
