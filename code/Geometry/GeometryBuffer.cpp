#include "GeometryBuffer.hpp"
#include "GeometryFormats.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../StateCache.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

bool GeometryBuffer::Create(IGeometryBuffer::CreationParams const& params) {
    if(params.vertexBuffer == nullptr) return false;
    if(params.indexBuffer == nullptr) return false;

    if(! (_SetVertexBuffer(params.vertexBuffer) && _SetIndexBuffer(params.indexBuffer)) ) return false;

    _drawMode = params.drawMode;

    return true;
}

bool GeometryBuffer::_SetVertexBuffer(VertexBufferPtr const& vb) {
    AssertAndExec(vb != nullptr, return false);
    _vb = vb;

    if(mr::gl::IsNVVBUMSupported()) {
        uint64_t residentPtr = 0;
        auto* buf = vb->GetBufferRange()->GetBuffer();
        if(buf->GetGPUAddress(residentPtr)) ;
        else buf->MakeResident();
    }

    return true;
}

bool GeometryBuffer::_SetIndexBuffer(IndexBufferPtr const& ib) {
    _ib = ib;

    if(ib != nullptr) {
        if(mr::gl::IsNVVBUMSupported()) {
            uint64_t residentPtr = 0;
            auto* buf = ib->GetBufferRange()->GetBuffer();
            if(buf->GetGPUAddress(residentPtr)) ;
            else buf->MakeResident();
        }
    }

    return true;
}

void GeometryBuffer::SetAttribute(VertexAttribute const& attr, IBuffer* buf) {
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
            IBuffer* binded = nullptr;
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
    if(!_vb) return false;

    StateCache* stateCache = StateCache::GetDefault();

    auto* buffer = _vb->GetBufferRange()->GetBuffer();
    auto vformat = _vb->GetFormat();

    if(!stateCache->SetVertexBuffer(buffer, vformat)) {
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

    stateCache->BindBuffer(buffer, StateCache::ArrayBuffer);

    if(mr::gl::IsNVVBUMSupported()) {
        mu::ArrayHandle<VertexAttribute> const& attrs = vformat->attributes;
        mu::ArrayHandle<uint64_t> const& ptrs = vformat->pointers;

        for(size_t i = 0; i < attrs.GetNum(); ++i) {
            uint64_t residentPtr = 0;
            if(buffer->GetGPUAddress(residentPtr)) {
                glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                       attrs.GetArray()[i].desc->shaderIndex,
                                       residentPtr + ptrs.GetArray()[i],
                                       buffer->GetGPUMem()
                                       );
            }
        }

        if(useIndexBuffer && _ib != nullptr){
            auto ibuffer = _ib->GetBufferRange()->GetBuffer();
            if(ibuffer != nullptr) {
                auto iformat = _ib->GetFormat();
                stateCache->SetIndexFormat(iformat);
                uint64_t residentPtr = 0;
                if(ibuffer->GetGPUAddress(residentPtr)) {
                    glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, residentPtr, ibuffer->GetGPUMem());
                }
            }
        }
    } else {
        if(useIndexBuffer && _ib != nullptr){
            auto ibuffer = _ib->GetBufferRange()->GetBuffer();
            if(ibuffer != nullptr) {
                auto iformat = _ib->GetFormat();
                stateCache->SetIndexBuffer(ibuffer, iformat);
            }
        }
    }

    return true;
}

GeometryBuffer::GeometryBuffer() :
    _vb(nullptr), _ib(nullptr),
    _drawMode((DrawMode)0)
{  }

GeometryBuffer::~GeometryBuffer(){
}

}
