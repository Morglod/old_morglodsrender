#include "GeometryFormats.hpp"

#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"

#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

namespace MR {

VertexDataTypeCustom::VertexDataTypeCustom(const unsigned int& gpu_type, const unsigned char& size) :
    _gpu_type(gpu_type), _size(size) {}

VertexAttributeCustom::VertexAttributeCustom(const unsigned char& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex) :
    _el_num(elementsNum), _data_type(dataType), _size( ((uint64_t)_el_num) * ((uint64_t)_data_type->Size()) ), _shaderIndex(shaderIndex) {}

void VertexFormatCustom::AddVertexAttribute(IVertexAttribute* a) {
    if(_pointers.size() == 0) _pointers.push_back(0);
    else _pointers.push_back(_nextPtr);
    _attribs.push_back(a);
    _size += (uint64_t)a->Size();
    _nextPtr = (uint64_t)_size;
}

bool VertexFormatCustom::Bind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glVertexAttribFormatNV(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size());
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glVertexAttribPointer(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size(), (void*)_pointers[i]);
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
        }
    }

    return true;
}

void VertexFormatCustom::UnBind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }

        glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    } else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }
    }
}

bool VertexFormatCustom::Equal(IVertexFormat* vf){
    if(Size() != vf->Size()) return false;

    TStaticArray<IVertexAttribute*> attrArray = vf->_Attributes();

    size_t i2 = 0;
    for(size_t i = 0; i < attrArray.GetNum(); ++i){
        if( !(_attribs[i]->Equal(attrArray.At(i2))) ) return false;
        ++i2;
    }
    return true;
}

VertexFormatCustom::VertexFormatCustom() : _attribs(), _pointers(), _nextPtr(0), _size(0) {
}

VertexFormatCustom::~VertexFormatCustom(){
}

void VertexFormatCustomFixed::SetVertexAttribute(IVertexAttribute* a, const size_t& index) {
    if(index == _attribsNum) return;
    _attribs[index] = a;
    this->_RecalcSize();
}

bool VertexFormatCustomFixed::Bind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribsNum; ++i) {
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
            glVertexAttribFormatNV(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribsNum; ++i) {
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
            glVertexAttribPointer(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size(), (void*)_pointers[i]);
            //glVertexAttribPointer(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, 0, (void*)_pointers[i]);
        }
    }

    return true;
}

void VertexFormatCustomFixed::UnBind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribsNum; ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }

        glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    } else {
        for(size_t i = 0; i < _attribsNum; ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }
    }
}

bool VertexFormatCustomFixed::Equal(IVertexFormat* vf) {
    if(_size != vf->Size()) return false;

    TStaticArray<IVertexAttribute*> attrArray = vf->_Attributes();

    size_t i2 = 0;
    for(size_t i = 0; i < attrArray.GetNum(); ++i){
        if( !(_attribs[i]->Equal(attrArray.At(i2))) ) return false;
        ++i2;
    }
    return true;
}

void VertexFormatCustomFixed::SetAttributesNum(const size_t& num) {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;

    _attribs = new IVertexAttribute*[num];
    _pointers = new uint64_t[num];
    _size = 0;
    _nextIndex = 0;
    _attribsNum = num;
}

void VertexFormatCustomFixed::_RecalcSize() {
    _size = 0;
    for(size_t i = 0; i < _nextIndex+1; ++i) {
        _pointers[i] = (uint64_t)_size;
        _size += _attribs[i]->Size();
    }
}

VertexFormatCustomFixed::VertexFormatCustomFixed() : _attribs(nullptr), _pointers(nullptr), _size(0), _nextIndex(0), _attribsNum(0) {}

VertexFormatCustomFixed::~VertexFormatCustomFixed() {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;
}

bool IndexFormatCustom::Bind(){
    if(MR::MachineInfo::IsNVVBUMSupported()){
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
    return true;
}

void IndexFormatCustom::UnBind(){
    if(MR::MachineInfo::IsNVVBUMSupported()){
        glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
}

bool IndexFormatCustom::Equal(IIndexFormat* ifo) {
    if(ifo->Size() != this->Size()) return false;
    return (ifo->GetDataType()->Equal(this->GetDataType()));
}

IndexFormatCustom::IndexFormatCustom(IVertexDataType* dataType) : _dataType(dataType) {
}

IndexFormatCustom::~IndexFormatCustom(){
}

}
