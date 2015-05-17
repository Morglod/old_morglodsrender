#include "GeometryBuffer.hpp"
#include "GeometryFormats.hpp"

#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../StateCache.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::IGeometryBuffer* _MR_BINDED_GEOM_BUFFER_ = nullptr;

void __MR_SET_VERTEX_FORMAT_AS_BINDED_(mr::VertexFormatPtr vf);
void __MR_SET_INDEX_FORMAT_AS_BINDED_(mr::IndexFormatPtr fi);

namespace mr {

bool GeometryBuffer::Create(IGeometryBuffer::CreationParams const& params) {
    if(params.vb == nullptr) return false;
    if(params.fv == nullptr) return false;

    _vb_nv_resident_ptr = _ib_nv_resident_ptr = _vb_nv_buffer_size = _ib_nv_buffer_size = 0;

    SetVertexBuffer(params.vb);
    SetIndexBuffer(params.ib);
    SetFormat(params.fv, params.fi);
    SetDrawMode(params.drawMode);

    _created = true;

    return true;
}

bool GeometryBuffer::SetVertexBuffer(IGPUBuffer* buf) {
    AssertAndExec(buf != nullptr, return false);
    _vb = buf;

    if(buf != nullptr) {
        if(mr::gl::IsNVVBUMSupported()){
            if(mr::gl::IsDSA_EXT()){
                glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
                glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_vb_nv_buffer_size);
            } else {
                StateCache* stateCache = StateCache::GetDefault();
                IGPUBuffer* binded = nullptr;
                if(!stateCache->ReBindBuffer(buf, StateCache::ArrayBuffer, &binded)) {
                    mr::Log::LogString("Bind buffer failed in GeometryBuffer::SetVertexBuffer.", MR_LOG_LEVEL_ERROR);
                    return false;
                }
                glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
                glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_vb_nv_buffer_size);
                if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
            }
        }
    }
    else {
        _vb_nv_resident_ptr = 0;
        _vb_nv_buffer_size = 0;
    }

    return true;
}

bool GeometryBuffer::SetIndexBuffer(IGPUBuffer* buf) {
    _ib = buf;

    if(buf != nullptr) {
        if(mr::gl::IsNVVBUMSupported()){
            if(mr::gl::IsDSA_EXT()){
                glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
                glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_ib_nv_buffer_size);
            } else {
                StateCache* stateCache = StateCache::GetDefault();
                IGPUBuffer* binded = nullptr;
                if(!stateCache->ReBindBuffer(buf, StateCache::ElementArrayBuffer, &binded)) {
                    mr::Log::LogString("Bind buffer failed in GeometryBuffer::SetIndexBuffer.", MR_LOG_LEVEL_ERROR);
                    return false;
                }
                glGetBufferParameterui64vNV(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
                glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &_ib_nv_buffer_size);
                if(binded) stateCache->BindBuffer(binded, StateCache::ElementArrayBuffer);
            }
        }
    }
    else {
        _ib_nv_resident_ptr = 0;
        _ib_nv_buffer_size = 0;
    }

    return true;
}

void GeometryBuffer::SetAttribute(VertexAttribute const& attr, IGPUBuffer* buf) {
    _customAttribs[attr] = buf;
    if(buf == nullptr) {
        auto it = _customAttribs.find(attr);
        _customAttribs.erase(it);
        auto it2 = _customAttribNVPtr.find(attr);
        _customAttribNVPtr.erase(it2);
        return;
    }
    if(mr::gl::IsNVVBUMSupported()) {
        BufferResidentPtr brp;
        if(mr::gl::IsDSA_EXT()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &brp.ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &brp.size);
        } else {
            StateCache* stateCache = StateCache::GetDefault();
            IGPUBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(buf, StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GeometryBuffer::SetAttribute.", MR_LOG_LEVEL_ERROR);
                return;
            }
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &brp.ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &brp.size);
            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
        _customAttribNVPtr[attr] = brp;
    }
}

bool GeometryBuffer::Bind(bool useIndexBuffer) const {
    //if(_MR_BINDED_GEOM_BUFFER_ == dynamic_cast<const mr::IGeometryBuffer*>(this)) return true;

    if(!_format) return false;
    if(!_vb) return false;

    StateCache* stateCache = StateCache::GetDefault();
    if(!stateCache->SetVertexBuffer(_vb, _format)) {
        mr::Log::LogString("Bind vertex buffer failed in GeometryBuffer::Bind.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    for(std::pair<VertexAttribute, IGPUBuffer*> const& cAttrs : _customAttribs) {
        //cAttrs.second->Bind(IGPUBuffer::ArrayBuffer);
        stateCache->BindBuffer(cAttrs.second, StateCache::ArrayBuffer);
        stateCache->BindVertexAttribute(cAttrs.first, cAttrs.first.desc->size);
    }

    for(std::pair<VertexAttribute, BufferResidentPtr> const& cAttrsPtr : _customAttribNVPtr) {
        glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   cAttrsPtr.first.desc->shaderIndex,
                                   cAttrsPtr.second.ptr,
                                   cAttrsPtr.second.size
                                   );
    }

    stateCache->BindBuffer(_vb, StateCache::ArrayBuffer);

    if(mr::gl::IsNVVBUMSupported()) {
        mu::ArrayHandle<VertexAttribute> const& attrs = _format->attributes;
        mu::ArrayHandle<uint64_t> const& ptrs = _format->pointers;

        for(size_t i = 0; i < attrs.GetNum(); ++i) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   attrs.GetArray()[i].desc->shaderIndex,
                                   _vb_nv_resident_ptr + ptrs.GetArray()[i],
                                   _vb_nv_buffer_size
                                   );
        }

        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            stateCache->SetIndexFormat(_iformat);
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, _ib_nv_resident_ptr, _ib_nv_buffer_size);
        }
    } else {
        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            stateCache->SetIndexBuffer(_ib, _iformat);
        }
    }

    _MR_BINDED_GEOM_BUFFER_ = (mr::IGeometryBuffer*)dynamic_cast<const mr::IGeometryBuffer*>(this);
    return true;
}

GeometryBuffer::GeometryBuffer() :
    _created(false),
    _vb(nullptr), _ib(nullptr),
    _format(nullptr), _iformat(nullptr),
    _draw_mode((mr::IGeometryBuffer::DrawMode)0),
    _vb_nv_resident_ptr(0), _ib_nv_resident_ptr(0),
    _vb_nv_buffer_size(0), _ib_nv_buffer_size(0)
{  }

GeometryBuffer::~GeometryBuffer(){
}

IGeometryBuffer* GeometryBufferGetBinded() {
    return _MR_BINDED_GEOM_BUFFER_;
}

}
