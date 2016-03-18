#pragma once

#include "mr/build.hpp"
#include "mr/draw.hpp"
#include "mr/vbuffer.hpp"
#include "mr/ibuffer.hpp"

namespace mr {

typedef std::shared_ptr<class Primitive> PrimitivePtr;

class MR_API Primitive {
    friend class _Alloc;
public:
    inline DrawMode GetDrawMode() const;
    inline VertexBufferPtr GetVertexBuffer() const;
    inline IndexBufferPtr GetIndexBuffer() const;
    inline uint32_t GetVertexBufferOffset() const;
    inline uint32_t GetIndexBufferOffset() const;

    // TODO inline bool Equal(PrimitivePtr const& other) const;

    virtual bool Draw(ShaderProgramPtr const& program, uint32_t instancesNum = 1) const;

    static PrimitivePtr Create(DrawMode const& dmode, VertexBufferPtr const& vb, uint32_t vb_offset = 0, IndexBufferPtr const& ib = nullptr, uint32_t ib_offset = 0, uint32_t baseVertex = 0, uint32_t baseIndex = 0, uint32_t baseInstance = 0);

    virtual ~Primitive() = default;
private:
    Primitive() = default;
    DrawMode _drawMode;
    VertexBufferPtr _vbuffer;
    IndexBufferPtr _ibuffer;
    uint32_t _vb_offset = 0;
    uint32_t _ib_offset = 0;
    uint32_t _baseVertex = 0;
    uint32_t _baseIndex = 0;
    uint32_t _baseInstance = 0;
};

inline DrawMode Primitive::GetDrawMode() const {
    return _drawMode;
}

inline VertexBufferPtr Primitive::GetVertexBuffer() const {
    return _vbuffer;
}

inline IndexBufferPtr Primitive::GetIndexBuffer() const {
    return _ibuffer;
}

inline uint32_t Primitive::GetVertexBufferOffset() const {
    return _vb_offset;
}

inline uint32_t Primitive::GetIndexBufferOffset() const {
    return _ib_offset;
}

/*
inline bool Primitive::Equal(PrimitivePtr const& other) const {
    if(_drawMode != other->_drawMode) return false;
    if(_instancesNum != other->_instancesNum) return false;
    if(_vbuffer == nullptr && other->_vbuffer != nullptr) return false;
    if(_vbuffer != nullptr && other->_vbuffer == nullptr) return false;
    if(_ibuffer == nullptr && other->_ibuffer != nullptr) return false;
    if(_ibuffer != nullptr && other->_ibuffer == nullptr) return false;
    if(!(_vbuffer == nullptr && _vbuffer == other->_vbuffer))
        if(_vbuffer->GetId() != other->_vbuffer->GetId()) return false;
    if(!(_ibuffer == nullptr && _ibuffer == other->_ibuffer))
        if(_ibuffer->GetId() != other->_ibuffer->GetId()) return false;
    return  true;
}*/

}
