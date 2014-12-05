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
    Assert(buf == nullptr)
    _vb = buf;

    if(mr::MachineInfo::IsNVVBUMSupported()){
        if(mr::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_vb_nv_buffer_size);
        } else {
            IGPUBuffer* binded = buf->ReBind(IGPUBuffer::ArrayBuffer);
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_vb_nv_resident_ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_vb_nv_buffer_size);
            if(binded) binded->Bind(IGPUBuffer::ArrayBuffer);
        }
    }
    return true;
}

bool GeometryBuffer::SetIndexBuffer(IGPUBuffer* buf) {
    _ib = buf;

    if(mr::MachineInfo::IsNVVBUMSupported()){
        if(mr::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_ib_nv_buffer_size);
        } else {
            IGPUBuffer* binded = buf->ReBind(IGPUBuffer::ElementArrayBuffer);
            glGetBufferParameterui64vNV(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_ib_nv_resident_ptr);
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &_ib_nv_buffer_size);
            if(binded) binded->Bind(IGPUBuffer::ElementArrayBuffer);
        }
    }

    return true;
}

bool GeometryBuffer::Bind(bool useIndexBuffer) {
    if(_MR_BINDED_GEOM_BUFFER_ == dynamic_cast<mr::IGeometryBuffer*>(this)) return true;

    if(!_format) return false;
    if(!_vb) return false;

    if(mr::MachineInfo::IsNVVBUMSupported()) {
        _format->Bind();//__MR_SET_VERTEX_FORMAT_AS_BINDED_(_format);

        TStaticArray<IVertexAttribute*> attrs = _format->_GetAttributes();
        TStaticArray<uint64_t> ptrs = _format->_GetOffsets();

        for(size_t i = 0; i < attrs.GetNum(); ++i) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   attrs.At(i)->GetShaderIndex(),
                                   _vb_nv_resident_ptr + ptrs.At(i),
                                   _vb_nv_buffer_size
                                   );
        }

        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            _iformat->Bind();
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, _ib_nv_resident_ptr, _ib_nv_buffer_size);
        }
    } else {
        _vb->Bind(IGPUBuffer::ArrayBuffer);
        _format->Bind();

        if(_ib != nullptr && useIndexBuffer && _iformat != nullptr){
            _ib->Bind(IGPUBuffer::ElementArrayBuffer);
            _iformat->Bind();
        }
    }

    _MR_BINDED_GEOM_BUFFER_ = dynamic_cast<mr::IGeometryBuffer*>(this);
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
