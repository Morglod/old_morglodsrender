#include "GeometryBuffer.hpp"

#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::IGeometryBuffer* _MR_BINDED_GEOM_BUFFER_ = nullptr;

void __MR_SET_VERTEX_FORMAT_AS_BINDED_(mr::IVertexFormat* vf);
void __MR_SET_INDEX_FORMAT_AS_BINDED_(mr::IIndexFormat* fi);

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
    Assert(buf != nullptr);
    _vb = buf;

    if(buf != nullptr) {
        if(mr::gl::IsNVVBUMSupported()){
            if(mr::gl::IsDirectStateAccessSupported()){
                glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
                glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_vb_nv_buffer_size);
            } else {
                IGPUBuffer* binded = buf->ReBind(IGPUBuffer::ArrayBuffer);
                glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
                glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_vb_nv_buffer_size);
                if(binded) binded->Bind(IGPUBuffer::ArrayBuffer);
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
            if(mr::gl::IsDirectStateAccessSupported()){
                glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
                glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_ib_nv_buffer_size);
            } else {
                IGPUBuffer* binded = buf->ReBind(IGPUBuffer::ElementArrayBuffer);
                glGetBufferParameterui64vNV(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
                glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &_ib_nv_buffer_size);
                if(binded) binded->Bind(IGPUBuffer::ElementArrayBuffer);
            }
        }
    }
    else {
        _ib_nv_resident_ptr = 0;
        _ib_nv_buffer_size = 0;
    }

    return true;
}

void GeometryBuffer::SetAttribute(IVertexAttribute* attr, IGPUBuffer* buf) {
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
        if(mr::gl::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &brp.ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &brp.size);
        } else {
            IGPUBuffer* binded = buf->ReBind(IGPUBuffer::ArrayBuffer);
            glGetBufferParameterui64vNV(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &brp.ptr);
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &brp.size);
            if(binded) binded->Bind(IGPUBuffer::ArrayBuffer);
        }
        _customAttribNVPtr[attr] = brp;
    }
}

bool GeometryBuffer::Bind(bool useIndexBuffer) const {
    if(_MR_BINDED_GEOM_BUFFER_ == dynamic_cast<const mr::IGeometryBuffer*>(this)) return true;

    if(!_format) return false;
    if(!_vb) return false;

    auto wasBinded = _vb->ReBind(IGPUBuffer::ArrayBuffer);
    _format->Bind();

    for(std::pair<IVertexAttribute*, IGPUBuffer*> const& cAttrs : _customAttribs) {
        cAttrs.second->Bind(IGPUBuffer::ArrayBuffer);
        unsigned int sh_i = cAttrs.first->GetShaderIndex();
        glEnableVertexAttribArray(sh_i);
        int elNum = (int)(cAttrs.first->GetElementsNum());
        auto dataTypePtr = cAttrs.first->GetDataType();

        if(mr::gl::IsNVVBUMSupported())
            glVertexAttribFormatNV(sh_i, elNum, dataTypePtr->GetDataType(), GL_FALSE, cAttrs.first->GetSize());
        else
            glVertexAttribPointer(sh_i, elNum, dataTypePtr->GetDataType(), GL_FALSE, cAttrs.first->GetSize(), 0);

        glVertexAttribDivisor(sh_i, cAttrs.first->GetDivisor());
    }

    for(std::pair<IVertexAttribute*, BufferResidentPtr> const& cAttrsPtr : _customAttribNVPtr) {
        glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   cAttrsPtr.first->GetShaderIndex(),
                                   cAttrsPtr.second.ptr,
                                   cAttrsPtr.second.size
                                   );
    }

    _vb->Bind(IGPUBuffer::ArrayBuffer);

    if(mr::gl::IsNVVBUMSupported()) {
        mu::ArrayRef<IVertexAttribute*> attrs = _format->GetAttributes();
        mu::ArrayRef<uint64_t> ptrs = _format->GetOffsets();

        for(size_t i = 0; i < attrs.num; ++i) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   attrs.elements[i]->GetShaderIndex(),
                                   _vb_nv_resident_ptr + ptrs.elements[i],
                                   _vb_nv_buffer_size
                                   );
        }

        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            _iformat->Bind();
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, _ib_nv_resident_ptr, _ib_nv_buffer_size);
        }
    } else {
        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            _ib->Bind(IGPUBuffer::ElementArrayBuffer);
            _iformat->Bind();
        }
    }

    if(wasBinded) wasBinded->Bind(IGPUBuffer::ArrayBuffer);

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
