#include "GeometryFormats.hpp"

#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"

#define GLEW_STATIC
#include <GL\glew.h>

MR::IVertexFormat* _MR_BINDED_VERTEX_FORMAT = nullptr;
MR::IIndexFormat* _MR_BINDED_INDEX_FORMAT = nullptr;

void __MR_SET_VERTEX_FORMAT_AS_BINDED_(MR::IVertexFormat* vf) {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) _MR_BINDED_VERTEX_FORMAT->UnBind();
    _MR_BINDED_VERTEX_FORMAT = vf;
}

void __MR_SET_INDEX_FORMAT_AS_BINDED_(MR::IIndexFormat* fi) {
    if(_MR_BINDED_INDEX_FORMAT != nullptr) _MR_BINDED_INDEX_FORMAT->UnBind();
    _MR_BINDED_INDEX_FORMAT = fi;
}

MR::TDynamicArray<MR::VertexDataTypeCustomCached> __MR_VERTEX_DATA_TYPE_CACHED_;
MR::TDynamicArray<MR::VertexAttributeCustomCached> __MR_VERTEX_ATTRIB_CACHED_;

namespace MR {

VertexDataTypeCustom::VertexDataTypeCustom() :
    _data_type(0), _size(0) {}

VertexDataTypeCustom::VertexDataTypeCustom(const unsigned int& data_type, const unsigned int& size) :
    _data_type(data_type), _size(size) {}

IVertexDataType* VertexDataTypeCustom::Cache() {
    auto ar = __MR_VERTEX_DATA_TYPE_CACHED_.GetRaw();
    MR::IVertexDataType* this_ptr = dynamic_cast<MR::IVertexDataType*>(this);
    for(size_t i = 0; i < __MR_VERTEX_DATA_TYPE_CACHED_.GetNum(); ++i) {
        if(ar[i].Equal(this_ptr)) {
            return ar[i].GetReal();
        }
    }
    VertexDataTypeCustomCached cached = VertexDataTypeCustomCached(this_ptr);
    __MR_VERTEX_DATA_TYPE_CACHED_.PushBack(cached);
    return cached.GetReal();
}

VertexDataTypeCustomCached::VertexDataTypeCustomCached(IVertexDataType* vdt) : _real(vdt) {}
VertexDataTypeCustomCached::VertexDataTypeCustomCached() : _real(nullptr) {}
VertexDataTypeCustomCached::VertexDataTypeCustomCached(VertexDataTypeCustomCached const& cpy) : _real(cpy._real) {}

VertexAttributeCustom::VertexAttributeCustom(const unsigned int& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex) :
    _el_num(elementsNum), _data_type(dataType), _size( ((uint64_t)_el_num) * ((uint64_t)_data_type->GetSize()) ), _shaderIndex(shaderIndex) {}

IVertexAttribute* VertexAttributeCustom::Cache() {
    auto ar = __MR_VERTEX_ATTRIB_CACHED_.GetRaw();
    MR::IVertexAttribute* this_ptr = dynamic_cast<MR::IVertexAttribute*>(this);
    for(size_t i = 0; i < __MR_VERTEX_ATTRIB_CACHED_.GetNum(); ++i) {
        if(ar[i].Equal(this_ptr)) {
            return ar[i].GetReal();
        }
    }
    VertexAttributeCustomCached cached = VertexAttributeCustomCached(this_ptr);
    __MR_VERTEX_ATTRIB_CACHED_.PushBack(cached);
    return cached.GetReal();
}

VertexAttributeCustomCached::VertexAttributeCustomCached() : _real(nullptr) {}
VertexAttributeCustomCached::VertexAttributeCustomCached(VertexAttributeCustomCached const& cpy) : _real(cpy._real) {}
VertexAttributeCustomCached::VertexAttributeCustomCached(IVertexAttribute* va) : _real(va) {}

void VertexFormatCustom::AddVertexAttribute(IVertexAttribute* a) {
    if(_pointers.size() == 0) _pointers.push_back(0);
    else _pointers.push_back(_nextPtr);
    _attribs.push_back(a);
    _size += (uint64_t)a->GetSize();
    _nextPtr = (uint64_t)_size;
}

bool VertexFormatCustom::Bind() {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) {
        if(_MR_BINDED_VERTEX_FORMAT->Equal(dynamic_cast<MR::IVertexFormat*>(this))) {
            return true;
        }
    }
    VertexFormatUnBind();

    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glVertexAttribFormatNV(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize());
            glEnableVertexAttribArray(sh_i);
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glVertexAttribPointer(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize(), (void*)_pointers[i]);
            glEnableVertexAttribArray(sh_i);
        }
    }

    _MR_BINDED_VERTEX_FORMAT = dynamic_cast<MR::IVertexFormat*>(this);
    return true;
}

void VertexFormatCustom::UnBind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->GetShaderIndex());
        }

        glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    } else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->GetShaderIndex());
        }
    }
    _MR_BINDED_VERTEX_FORMAT = nullptr;
}

bool VertexFormatCustom::Equal(IVertexFormat* vf){
    if(GetSize() != vf->GetSize()) return false;

    TStaticArray<IVertexAttribute*> attrArray = vf->_GetAttributes();

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
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) {
        if(_MR_BINDED_VERTEX_FORMAT->Equal(dynamic_cast<MR::IVertexFormat*>(this))) {
            return true;
        }
    }
    VertexFormatUnBind();

    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribsNum; ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glEnableVertexAttribArray(sh_i);
            glVertexAttribFormatNV(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribsNum; ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glEnableVertexAttribArray(sh_i);
            glVertexAttribPointer(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize(), (void*)_pointers[i]);
        }
    }

    _MR_BINDED_VERTEX_FORMAT = dynamic_cast<MR::IVertexFormat*>(this);
    return true;
}

void VertexFormatCustomFixed::UnBind() {
    if(MR::MachineInfo::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribsNum; ++i) {
            glDisableVertexAttribArray(_attribs[i]->GetShaderIndex());
        }

        glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    } else {
        for(size_t i = 0; i < _attribsNum; ++i) {
            glDisableVertexAttribArray(_attribs[i]->GetShaderIndex());
        }
    }
    _MR_BINDED_VERTEX_FORMAT = nullptr;
}

bool VertexFormatCustomFixed::Equal(IVertexFormat* vf) {
    if(_size != vf->GetSize()) return false;

    TStaticArray<IVertexAttribute*> attrArray = vf->_GetAttributes();

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
        _size += _attribs[i]->GetSize();
    }
}

VertexFormatCustomFixed::VertexFormatCustomFixed() : _attribs(nullptr), _pointers(nullptr), _size(0), _nextIndex(0), _attribsNum(0) {}

VertexFormatCustomFixed::~VertexFormatCustomFixed() {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;
}

bool IndexFormatCustom::Bind(){
    if(_MR_BINDED_INDEX_FORMAT != nullptr) {
        if(_MR_BINDED_INDEX_FORMAT->Equal(dynamic_cast<MR::IIndexFormat*>(this))) {
            return true;
        }
    }
    IndexFormatUnBind();

    if(MR::MachineInfo::IsNVVBUMSupported()){
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }

    _MR_BINDED_INDEX_FORMAT = dynamic_cast<MR::IIndexFormat*>(this);
    return true;
}

void IndexFormatCustom::UnBind(){
    if(MR::MachineInfo::IsNVVBUMSupported()){
        glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
    _MR_BINDED_INDEX_FORMAT = nullptr;
}

bool IndexFormatCustom::Equal(IIndexFormat* ifo) {
    if(ifo->GetSize() != this->GetSize()) return false;
    return (ifo->GetDataType()->Equal(this->GetDataType()));
}

IndexFormatCustom::IndexFormatCustom(IVertexDataType* dataType) : _dataType(dataType) {
}

IndexFormatCustom::~IndexFormatCustom(){
}

IVertexFormat* VertexFormatGetBinded() {
    return _MR_BINDED_VERTEX_FORMAT;
}

void VertexFormatUnBind() {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) {
        _MR_BINDED_VERTEX_FORMAT->UnBind();
    }
}

IIndexFormat* IndexFormatGetBinded() {
    return _MR_BINDED_INDEX_FORMAT;
}

void IndexFormatUnBind() {
    if(_MR_BINDED_INDEX_FORMAT != nullptr) {
        _MR_BINDED_INDEX_FORMAT->UnBind();
    }
}

}
