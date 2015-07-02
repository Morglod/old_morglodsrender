#include "GeometryBuffer.hpp"
#include "GeometryFormats.hpp"

#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../StateCache.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

bool GeometryBuffer::Create(IGeometryBuffer::CreationParams const& params) {
    if(params.vb == nullptr) return false;
    if(params.fv == nullptr) return false;

    SetVertexBuffer(params.vb);
    SetIndexBuffer(params.ib);
    SetFormat(params.fv, params.fi);
    SetDrawMode(params.drawMode);

    return true;
}

bool GeometryBuffer::SetVertexBuffer(IGPUBuffer* buf) {
    AssertAndExec(buf != nullptr, return false);
    _vb = buf;

    if(mr::gl::IsNVVBUMSupported()) {
        uint64_t residentPtr = 0;
        if(_vb->GetGPUAddress(residentPtr)) ;
        else _vb->MakeResident();
    }

    return true;
}

bool GeometryBuffer::SetIndexBuffer(IGPUBuffer* buf) {
    _ib = buf;

    if(mr::gl::IsNVVBUMSupported()) {
        uint64_t residentPtr = 0;
        if(_ib->GetGPUAddress(residentPtr)) ;
        else _ib->MakeResident();
    }

    return true;
}

void GeometryBuffer::SetAttribute(VertexAttribute const& attr, IGPUBuffer* buf) {
    if(buf == nullptr) {
        auto it = _customAttribs.find(attr);
        if(it != _customAttribs.end()) _customAttribs.erase(it);
        return;
    }

    CustomAttribute customAttrib;
    customAttrib.buffer = buf;

    if(mr::gl::IsNVVBUMSupported()) {
        if(mr::gl::IsDSA_EXT()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &customAttrib.ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &customAttrib.size);
        } else {
            StateCache* stateCache = StateCache::GetDefault();
            IGPUBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(buf, StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GeometryBuffer::SetAttribute.", MR_LOG_LEVEL_ERROR);
                return;
            }
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &customAttrib.ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &customAttrib.size);
            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
    }
    _customAttribs[attr] = customAttrib;
}

bool GeometryBuffer::Bind(bool useIndexBuffer) const {
    //TODO cache
    if(!_format) return false;
    if(!_vb) return false;

    StateCache* stateCache = StateCache::GetDefault();
    if(!stateCache->SetVertexBuffer(_vb, _format)) {
        mr::Log::LogString("Bind vertex buffer failed in GeometryBuffer::Bind.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    for(std::pair<VertexAttribute, CustomAttribute> const& cAttrs : _customAttribs) {
        CustomAttribute const& attr = cAttrs.second;
        stateCache->BindBuffer(attr.buffer, StateCache::ArrayBuffer);
        stateCache->BindVertexAttribute(cAttrs.first, cAttrs.first.desc->size);

        if(attr.ptr != 0) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                       cAttrs.first.desc->shaderIndex,
                                       attr.ptr,
                                       attr.size
                                       );
        }
    }

    stateCache->BindBuffer(_vb, StateCache::ArrayBuffer);

    if(mr::gl::IsNVVBUMSupported()) {
        mu::ArrayHandle<VertexAttribute> const& attrs = _format->attributes;
        mu::ArrayHandle<uint64_t> const& ptrs = _format->pointers;

        for(size_t i = 0; i < attrs.GetNum(); ++i) {
            uint64_t residentPtr = 0;
            if(_vb->GetGPUAddress(residentPtr)) {
                glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                       attrs.GetArray()[i].desc->shaderIndex,
                                       residentPtr + ptrs.GetArray()[i],
                                       _vb->GetGPUMem()
                                       );
            }
        }

        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            stateCache->SetIndexFormat(_iformat);
            uint64_t residentPtr = 0;
            if(_ib->GetGPUAddress(residentPtr)) {
                glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, residentPtr, _ib->GetGPUMem());
            }
        }
    } else {
        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            stateCache->SetIndexBuffer(_ib, _iformat);
        }
    }

    return true;
}

GeometryBuffer::GeometryBuffer() :
    _vb(nullptr), _ib(nullptr),
    _format(nullptr), _iformat(nullptr),
    _draw_mode((mr::IGeometryBuffer::DrawMode)0)
{  }

GeometryBuffer::~GeometryBuffer(){
}

}
