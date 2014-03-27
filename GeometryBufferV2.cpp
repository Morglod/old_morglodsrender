#include "GeometryBufferV2.hpp"
#include "MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

IVertexDataType* VertexDataTypeInt::Instance(){
    static IVertexDataType* inst = new VertexDataTypeInt;
    return inst;
}

IVertexDataType* VertexDataTypeUInt::Instance(){
    static IVertexDataType* inst = new VertexDataTypeUInt;
    return inst;
}

IVertexDataType* VertexDataTypeFloat::Instance(){
    static IVertexDataType* inst = new VertexDataTypeFloat;
    return inst;
}

VertexDataTypeCustom::VertexDataTypeCustom(const unsigned int& gpu_type, const unsigned char& size) :
    _gpu_type(gpu_type), _size(size) {}

VertexAttributeCustom::VertexAttributeCustom(const unsigned char& elementsNum, IVertexDataType* dataType) :
    _el_num(elementsNum), _data_type(dataType), _size( ((unsigned int)_el_num) * ((unsigned int)_data_type->Size()) ) {}

void VertexFormatCustom::SetVertexAttributes(const MR::Array<IVertexAttribute*>& va) {
    _va = va;
    _size = 0;
    for(size_t i = 0; i < _va.Size(); ++i) {
        _size += _va[i]->Size();
    }
}

VertexFormatCustom::VertexFormatCustom(const MR::Array<IVertexAttribute*>& va) : _va(va) {
    SetVertexAttributes(va);
}

bool VertexBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag){
    _usage = usage;
    _accessFlag = accessFlag;
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
    }
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, data, usage);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if(MR::MachineInfo::gpu_vendor() == MR::MachineInfo::GPUVendor::Nvidia){
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
        glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
        glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_buffer_size);
        glMakeBufferResidentNV(GL_ARRAY_BUFFER, accessFlag);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    return true;
}

void VertexBuffer::Release(){
    if(_handle != 0){
        glDeleteBuffers(1, &_handle);
        _handle = 0;
    }
}

VertexBuffer::VertexBuffer() : Super(), _usage(0), _accessFlag(0), _handle(0) {
}

VertexBuffer::~VertexBuffer(){
    Release();
}

}
