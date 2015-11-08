#include "src/statecache.hpp"
#include "mr/log.hpp"
#include "mr/pre/glew.hpp"

namespace {

mr::StateCache _state_cache;

}

namespace mr {

StateCache::_VertexBufferBindings::_VertexBufferBindings(uint32_t n) {
    ar = new _VertexBuffer[n];
    num = n;
}

StateCache::_VertexBufferBindings::~_VertexBufferBindings() {
    if(ar == nullptr) {
        delete [] ar;
        ar = nullptr;
        num = 0;
    }
}

StateCache* StateCache::Get() {
    return &_state_cache;
}

bool StateCache::SetShaderProgram(uint32_t program) {
    if(_program == program) return false;
    _program = program;
    return true;
}

bool StateCache::SetVertexBuffer(uint32_t bindpoint, uint32_t vb, uint32_t offset, uint32_t stride, VertexDecl* decl) {
    if(bindpoint >= _vb->num) {
        MR_LOG_ERROR(StateCache::SetVertexBuffer, "bindpoint >= totalBindpoints");
        return false;
    }

    _VertexBuffer vb_cache = _VertexBuffer{offset, vb, stride, decl};
    auto& bp_ref = _vb->ar[bindpoint];
    if(bp_ref != vb_cache) {
        bp_ref = vb_cache;
        return true;
    }
    return false;
}

bool StateCache::GetVertexBuffer(uint32_t bindpoint, uint32_t& out_vb, uint32_t& out_offset, uint32_t& out_stride, VertexDecl*& out_decl) const {
    if(bindpoint >= _vb->num) {
        MR_LOG_WARNING(StateCache::GetVertexBuffer, "bindpoint >= totalBindpoints");
        return false;
    }

    const auto cache = _vb->ar[bindpoint];
    out_offset = cache.offset;
    out_vb = cache.buffer;
    out_stride = cache.stride;
    out_decl = cache.decl;

    return true;
}

bool StateCache::_Init() {
    {   // Init VertexBuffer bindpoints
        int num;
        glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS, &num);
        if(num <= 0) {
            MR_LOG_ERROR(StateCache::_Init, "Failed get GL_MAX_VERTEX_ATTRIB_BINDINGS, it's value <= 0");
            return false;
        }
        _vb = std::make_unique<_VertexBufferBindings>(num);
    }

    return true;
}

}