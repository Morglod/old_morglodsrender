#include "GeometryFormats.hpp"

#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"

#define GLEW_STATIC
#include <GL\glew.h>

mr::IVertexFormat* _MR_BINDED_VERTEX_FORMAT = nullptr;
mr::IIndexFormat* _MR_BINDED_INDEX_FORMAT = nullptr;

void __MR_SET_VERTEX_FORMAT_AS_BINDED_(mr::IVertexFormat* vf) {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) _MR_BINDED_VERTEX_FORMAT->UnBind();
    _MR_BINDED_VERTEX_FORMAT = vf;
}

void __MR_SET_INDEX_FORMAT_AS_BINDED_(mr::IIndexFormat* fi) {
    if(_MR_BINDED_INDEX_FORMAT != nullptr) _MR_BINDED_INDEX_FORMAT->UnBind();
    _MR_BINDED_INDEX_FORMAT = fi;
}

//Handles std::shared_ptr for safe using with std::vector
namespace mr {

struct __VertexDataTypeCached {
    std::shared_ptr<IVertexDataType> _ptr;
    __VertexDataTypeCached() : _ptr(nullptr) {}
    __VertexDataTypeCached(IVertexDataType* vdt) : _ptr(vdt) {}
    __VertexDataTypeCached(__VertexDataTypeCached& cpy) : _ptr(cpy._ptr) {}
    __VertexDataTypeCached(__VertexDataTypeCached const& cpy) : _ptr(cpy._ptr) {}
};

struct __VertexAttributeCached {
    std::shared_ptr<IVertexAttribute> _ptr;
    __VertexAttributeCached() : _ptr(nullptr) {}
    __VertexAttributeCached(IVertexAttribute* va) : _ptr(va) {}
    __VertexAttributeCached(__VertexAttributeCached& cpy) : _ptr(cpy._ptr) {}
    __VertexAttributeCached(__VertexAttributeCached const& cpy) : _ptr(cpy._ptr) {}
};

struct __VertexFormatCached {
    std::shared_ptr<IVertexFormat> _ptr;
    __VertexFormatCached() : _ptr(nullptr) {}
    __VertexFormatCached(IVertexFormat* va) : _ptr(va) {}
    __VertexFormatCached(__VertexFormatCached& cpy) : _ptr(cpy._ptr) {}
    __VertexFormatCached(__VertexFormatCached const& cpy) : _ptr(cpy._ptr) {}
};

struct __IndexFormatCached {
    std::shared_ptr<IIndexFormat> _ptr;
    __IndexFormatCached() : _ptr(nullptr) {}
    __IndexFormatCached(IIndexFormat* va) : _ptr(va) {}
    __IndexFormatCached(__IndexFormatCached& cpy) : _ptr(cpy._ptr) {}
    __IndexFormatCached(__IndexFormatCached const& cpy) : _ptr(cpy._ptr) {}
};

}

//Cached objects
std::vector<mr::__VertexDataTypeCached> __MR_VERTEX_DATA_TYPE_CACHED_;
std::vector<mr::__VertexAttributeCached> __MR_VERTEX_ATTRIB_CACHED_;
std::vector<mr::__VertexFormatCached> __MR_VERTEX_FORMAT_CACHED_;
std::vector<mr::__IndexFormatCached> __MR_INDEX_FORMAT_CACHED_;

/*
    Cache function
It adds copy of 'what' to 'where' vector, if there is no any equal to 'what'.
Returns object from vector.

Example for MR::VertexDataTypeCustom

IVertexDataType* VertexDataTypeCustom::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<MR::__VertexDataTypeCached, IVertexDataType, VertexDataTypeCustom> (this, __MR_VERTEX_DATA_TYPE_CACHED_);
}

*/
template<typename CacheType, typename InterfaceType, typename Type>
InterfaceType* __MR_GEOM_FORMAT_CACHE_FUNC_(Type* what, std::vector<CacheType>& where) {
    CacheType * ar = &where[0];
    InterfaceType* inter = dynamic_cast<InterfaceType*>(what);
    for(size_t i = 0; i < where.size(); ++i) {
        if(ar[i]._ptr->IsEqual(inter)) {
            return ar[i]._ptr.get();
        }
    }
    InterfaceType* cpy = dynamic_cast<InterfaceType*>(new Type(*what));
    where.push_back( CacheType(cpy) );
    return cpy;
}

namespace mr {

IVertexDataType* VertexDataTypeCustom::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<mr::__VertexDataTypeCached, IVertexDataType, VertexDataTypeCustom> (this, __MR_VERTEX_DATA_TYPE_CACHED_);
}

VertexDataTypeCustom::VertexDataTypeCustom() :
    _data_type(GL_FLOAT), _size(0) {}

VertexDataTypeCustom::VertexDataTypeCustom(const unsigned int& data_type, const unsigned int& size) :
    _data_type(data_type), _size(size) {}

VertexDataTypeCustom::VertexDataTypeCustom(VertexDataTypeCustom const& cpy) :
    _data_type(cpy._data_type), _size(cpy._data_type) {}


IVertexAttribute* VertexAttributeCustom::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<mr::__VertexAttributeCached, IVertexAttribute, VertexAttributeCustom>(this, __MR_VERTEX_ATTRIB_CACHED_);
}

VertexAttributeCustom::VertexAttributeCustom() :
    _el_num(0), _data_type(nullptr), _size(0), _shaderIndex(0), _divisor(0) {}

VertexAttributeCustom::VertexAttributeCustom(const unsigned int& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex, unsigned const& divisor) :
    _el_num(elementsNum), _data_type(dataType), _size( ((uint64_t)_el_num) * ((uint64_t)_data_type->GetSize()) ), _shaderIndex(shaderIndex), _divisor(divisor) {}

VertexAttributeCustom::VertexAttributeCustom(VertexAttributeCustom const& cpy) :
    _el_num(cpy._el_num), _data_type(cpy._data_type), _size(cpy._size), _shaderIndex(cpy._shaderIndex), _divisor(cpy._divisor) {}


void VertexFormatCustom::AddVertexAttribute(IVertexAttribute* a) {
    if(_pointers.size() == 0) _pointers.push_back(0);
    else _pointers.push_back(_nextPtr);
    _attribs.push_back(a);
    _size += (uint64_t)a->GetSize();
    _nextPtr = (uint64_t)_size;
}

bool VertexFormatCustom::Bind() const {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) {
        if(_MR_BINDED_VERTEX_FORMAT->IsEqual((mr::IVertexFormat*)dynamic_cast<const mr::IVertexFormat*>(this))) {
            return true;
        }
    }
    VertexFormatUnBind();

    if(mr::gl::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glVertexAttribFormatNV(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize());
            glEnableVertexAttribArray(sh_i);
            glVertexAttribDivisor(sh_i, _attribs[i]->GetDivisor());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glVertexAttribPointer(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize(), (void*)_pointers[i]);
            glEnableVertexAttribArray(sh_i);
            glVertexAttribDivisor(sh_i, _attribs[i]->GetDivisor());
        }
    }

    _MR_BINDED_VERTEX_FORMAT = (mr::IVertexFormat*)dynamic_cast<const mr::IVertexFormat*>(this);
    return true;
}

void VertexFormatCustom::UnBind() const {
    if(mr::gl::IsNVVBUMSupported()){
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

bool VertexFormatCustom::IsEqual(IVertexFormat* vf) const {
    if(GetSize() != vf->GetSize()) return false;

    mu::ArrayRef<IVertexAttribute*> attrArray = vf->GetAttributes();

    size_t i2 = 0;
    for(size_t i = 0; i < attrArray.num; ++i){
        if( !(_attribs[i]->IsEqual(attrArray.elements[i2])) ) return false;
        ++i2;
    }
    return true;
}

IVertexFormat* VertexFormatCustom::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<mr::__VertexFormatCached, IVertexFormat, VertexFormatCustom>(this, __MR_VERTEX_FORMAT_CACHED_);
}

VertexFormatCustom::VertexFormatCustom() : _attribs(), _pointers(), _nextPtr(0), _size(0) {
}

VertexFormatCustom::VertexFormatCustom(VertexFormatCustom const& cpy) : _attribs(cpy._attribs), _pointers(cpy._pointers), _nextPtr(cpy._nextPtr), _size(cpy._size) {
}

VertexFormatCustom::~VertexFormatCustom(){
}

void VertexFormatCustomFixed::SetVertexAttribute(IVertexAttribute* a, const size_t& index) {
    if(index == _attribsNum) return;
    _attribs[index] = a;
    this->_RecalcSize();
}

bool VertexFormatCustomFixed::Bind() const {
    if(_MR_BINDED_VERTEX_FORMAT != nullptr) {
        if(_MR_BINDED_VERTEX_FORMAT->IsEqual((mr::IVertexFormat*)dynamic_cast<const mr::IVertexFormat*>(this))) {
            return true;
        }
    }
    VertexFormatUnBind();

    if(mr::gl::IsNVVBUMSupported()){
        for(size_t i = 0; i < _attribsNum; ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glEnableVertexAttribArray(sh_i);
            glVertexAttribFormatNV(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize());
            glVertexAttribDivisor(sh_i, _attribs[i]->GetDivisor());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribsNum; ++i) {
            unsigned int sh_i = _attribs[i]->GetShaderIndex();
            glEnableVertexAttribArray(sh_i);
            glVertexAttribPointer(sh_i, (int)_attribs[i]->GetElementsNum(), _attribs[i]->GetDataType()->GetDataType(), GL_FALSE, this->GetSize(), (void*)_pointers[i]);
            glVertexAttribDivisor(sh_i, _attribs[i]->GetDivisor());
        }
    }

    _MR_BINDED_VERTEX_FORMAT = (mr::IVertexFormat*)dynamic_cast<const mr::IVertexFormat*>(this);
    return true;
}

void VertexFormatCustomFixed::UnBind() const {
    if(mr::gl::IsNVVBUMSupported()){
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

bool VertexFormatCustomFixed::IsEqual(IVertexFormat* vf) const {
    if(_size != vf->GetSize()) return false;

    mu::ArrayRef<IVertexAttribute*> attrArray = vf->GetAttributes();

    size_t i2 = 0;
    for(size_t i = 0; i < attrArray.num; ++i){
        if( !(_attribs[i]->IsEqual(attrArray.elements[i2])) ) return false;
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

IVertexFormat* VertexFormatCustomFixed::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<mr::__VertexFormatCached, IVertexFormat, VertexFormatCustomFixed>(this, __MR_VERTEX_FORMAT_CACHED_);
}

VertexFormatCustomFixed::VertexFormatCustomFixed() : _attribs(nullptr), _pointers(nullptr), _size(0), _nextIndex(0), _attribsNum(0) {}

VertexFormatCustomFixed::VertexFormatCustomFixed(VertexFormatCustomFixed const& cpy)
    : _attribs(nullptr), _pointers(nullptr), _size(cpy._size), _nextIndex(cpy._nextIndex), _attribsNum(cpy._attribsNum)
{
    if(_attribsNum != 0) {
        _attribs = new IVertexAttribute*[_attribsNum];
        _pointers = new uint64_t[_attribsNum];
        for(unsigned int i = 0; i < _attribsNum; ++i) {
            _attribs[i] = cpy._attribs[i];
            _pointers[i] = cpy._pointers[i];
        }
    }
}

VertexFormatCustomFixed::~VertexFormatCustomFixed() {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;
}

bool IndexFormatCustom::Bind() const {
    if(_MR_BINDED_INDEX_FORMAT != nullptr) {
        if(_MR_BINDED_INDEX_FORMAT->IsEqual((mr::IIndexFormat*)dynamic_cast<const mr::IIndexFormat*>(this))) {
            return true;
        }
    }
    IndexFormatUnBind();

    if(mr::gl::IsNVVBUMSupported()){
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }

    _MR_BINDED_INDEX_FORMAT = (mr::IIndexFormat*)dynamic_cast<const mr::IIndexFormat*>(this);
    return true;
}

void IndexFormatCustom::UnBind() const {
    if(mr::gl::IsNVVBUMSupported()){
        glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
    _MR_BINDED_INDEX_FORMAT = nullptr;
}

bool IndexFormatCustom::IsEqual(IIndexFormat* ifo) const {
    if(ifo->GetSize() != this->GetSize()) return false;
    return (ifo->GetDataType()->IsEqual(this->GetDataType()));
}

IIndexFormat* IndexFormatCustom::Cache() {
    return __MR_GEOM_FORMAT_CACHE_FUNC_<__IndexFormatCached, IIndexFormat, IndexFormatCustom>(this, __MR_INDEX_FORMAT_CACHED_);
}

IndexFormatCustom::IndexFormatCustom() : _dataType(nullptr) {
}

IndexFormatCustom::IndexFormatCustom(IndexFormatCustom const& cpy) : _dataType(cpy._dataType) {
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
