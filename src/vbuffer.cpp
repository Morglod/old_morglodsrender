#include "mr/vbuffer.hpp"
#include "mr/buffer.hpp"
#include "mr/log.hpp"
#include "src/mp.hpp"
#include "src/statecache.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

bool VertexBuffer::Bind(uint32_t binding, uint32_t offset) {
    MP_ScopeSample(VertexBuffer::Bind);

    const uint32_t buffer = _vbuf->GetId();
    const uint32_t stride = _vdecl->GetSize();
    if(!StateCache::Get()->SetVertexBuffer(binding, buffer, offset, stride, _vdecl.get())) return true;

    // Don't bind buffer with VBUM, because it will be pointed by resident ptr.
    if(!GLEW_NV_vertex_buffer_unified_memory)
        glBindVertexBuffer(binding, buffer, offset, stride);

    if(!_vdecl->Bind()) {
        MR_LOG_ERROR(VertexBuffer::Bind, "Failed bind VertexDecl");
        return false;
    }

    // Address buffer's memory
    if(GLEW_NV_vertex_buffer_unified_memory) {
        const auto resident_ptr = _vbuf->_resident.address;
        const auto resident_size = _vbuf->_size;
        for(uint8_t i_bp = 0, n_bp = _vdecl->_map.num; i_bp < n_bp; ++i_bp) { //foreach bindpoint
            VertexDecl::BindPoint const& bindpoint = _vdecl->_map.bindpoints[i_bp];
            for(uint8_t i_a = 0, n_a = bindpoint.num; i_a < n_a; ++i_a) { //foreach attrib
                VertexDecl::Attrib const& attrib = bindpoint.attribs[i_a];
                glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV, attrib.index, resident_ptr + attrib.offset, resident_size - attrib.offset);
            }
        }
    }

    return true;
}

VertexBufferPtr VertexBuffer::Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) {
    MP_ScopeSample(VertexBuffer::Create);
    return VertexBufferPtr(new VertexBuffer(vbuf, vdecl, num));
}

VertexBuffer::VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) : _vbuf(vbuf), _vdecl(vdecl), _num(num) {
}

}
