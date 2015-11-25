#pragma once

#include "mr/build.hpp"

#include <string.h>
#include <memory>

namespace mr {

class VertexDecl;

class MR_API StateCache final {
    friend class Core;
public:
    /// On Set* return true, if need rebind
    /// Operate with GL handles

    bool SetShaderProgram(uint32_t program);
    inline uint32_t GetShaderProgram() const;

    bool SetVertexBuffer(uint32_t bindpoint, uint32_t vb, uint32_t offset, uint32_t stride, VertexDecl* decl);
    bool GetVertexBuffer(uint32_t bindpoint, uint32_t& out_vb, uint32_t& out_offset, uint32_t& out_stride, VertexDecl*& out_decl) const;

    inline bool SetIndexBuffer(uint32_t ib);
    inline uint32_t GetIndexBuffer() const;

    bool SetUniformBuffer(uint32_t buffer, uint32_t binding);
    bool GetUniformBuffer(uint32_t binding, uint32_t& out_buffer);

    static StateCache* Get();
protected:
    bool _Init();

    uint32_t _program = 0;

    struct _VertexBuffer {
        uint32_t offset;
        uint32_t buffer;
        uint32_t stride;
        VertexDecl* decl;

        inline bool operator != (_VertexBuffer& other) const;
    };

    struct _VertexBufferBindings {
        _VertexBuffer* ar = nullptr;
        uint32_t num = 0;

        _VertexBufferBindings(uint32_t num);
        ~_VertexBufferBindings();
    };

    // sizeof GL_MAX_VERTEX_ATTRIB_BINDINGS
    std::unique_ptr<_VertexBufferBindings> _vb = nullptr;

    uint32_t _ib = 0;

    struct _UniformBufferBindings {
        uint32_t* ar = nullptr;
        uint32_t num = 0;

        _UniformBufferBindings(uint32_t num);
        ~_UniformBufferBindings();
    };

    // sizeof GL_MAX_UNIFORM_BUFFER_BINDINGS
    std::unique_ptr<_UniformBufferBindings> _ubo = nullptr;
};

inline uint32_t StateCache::GetShaderProgram() const {
    return _program;
}

inline bool StateCache::_VertexBuffer::operator != (StateCache::_VertexBuffer& other) const {
    return memcmp(this, &other, sizeof(StateCache::_VertexBuffer)) != 0;
}

inline uint32_t StateCache::GetIndexBuffer() const {
    return _ib;
}

inline bool StateCache::SetIndexBuffer(uint32_t ib) {
    if(_ib != ib) {
        _ib = ib;
        return true;
    }
    return false;
}

}
