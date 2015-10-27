#include "mr/vbuffer.hpp"
#include "mr/buffer.hpp"
#include "mr/log.hpp"
#include "src/mp.hpp"
#include <unordered_map>

#include "mr/pre/glew.hpp"

namespace {

struct _VertexBufferBindCache {
    uint32_t offset;
    uint32_t buffer;
};

std::unordered_map<uint32_t, _VertexBufferBindCache> _vb_bind_cache; // <bindpoint, struct>

bool _VB_NeedRebind(uint32_t buffer, uint32_t binding, uint32_t offset) {
    if(_vb_bind_cache.count(binding) == 0) return true;
    _VertexBufferBindCache const& cache = _vb_bind_cache.at(binding);
    if(cache.buffer != buffer || cache.offset != offset) return true;
    return false;
}

void _VB_Cache(uint32_t buffer, uint32_t binding, uint32_t offset) {
    _vb_bind_cache[binding].offset = offset;
    _vb_bind_cache[binding].buffer = buffer;
}

}

namespace mr {

bool VertexBuffer::Bind(uint32_t binding, uint32_t offset) {
    MP_ScopeSample(VertexBuffer::Bind);

    const uint32_t buffer = _vbuf->GetId();
    if(!_VB_NeedRebind(buffer, binding, offset)) return true;

    // Don't bind buffer with VBUM, because it will be pointed by resident ptr.
    if(!GLEW_NV_vertex_buffer_unified_memory)
        glBindVertexBuffer(binding, buffer, offset, _vdecl->GetSize());

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

    _VB_Cache(buffer, binding, offset);

    return true;
}

VertexBufferPtr VertexBuffer::Create(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) {
    return VertexBufferPtr(new VertexBuffer(vbuf, vdecl, num));
}

VertexBuffer::VertexBuffer(BufferPtr const& vbuf, VertexDeclPtr const& vdecl, uint32_t num) : _vbuf(vbuf), _vdecl(vdecl), _num(num) {
}

}
