#pragma once

#ifndef _MR_GEOMETRY_FORMATS_H_
#define _MR_GEOMETRY_FORMATS_H_

#include "../Types.hpp"
#include "../Config.hpp"
#include "GeometryInterfaces.hpp"
#include "../Utils/Singleton.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace MR {

class VertexFormatCustom;
class VertexFormatCustomFixed;

class VertexDataTypeInt : public IVertexDataType, public StaticSingleton<VertexDataTypeInt> {
public:
    inline unsigned int GetSize() override { return sizeof(int); }
    inline unsigned int GetDataType() override { return 0x1404; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(int) != dt->GetSize()) return false;
        if(0x1404 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeUInt : public IVertexDataType, public StaticSingleton<VertexDataTypeUInt> {
public:
    inline unsigned int GetSize() override { return sizeof(unsigned int); }
    inline unsigned int GetDataType() override { return 0x1405; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(unsigned int) != dt->GetSize()) return false;
        if(0x1405 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeFloat : public IVertexDataType, public StaticSingleton<VertexDataTypeFloat> {
public:
    inline unsigned int GetSize() override { return sizeof(float); }
    inline unsigned int GetDataType() override { return 0x1406; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(float) != dt->GetSize()) return false;
        if(0x1406 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeCustom : public IVertexDataType {
public:
    inline unsigned int GetSize() override {return _size;}
    inline unsigned int GetDataType() override {return _data_type;}

    inline bool Equal(IVertexDataType* dt) override {
        if(_size != dt->GetSize()) return false;
        if(_data_type != dt->GetDataType()) return false;
        return true;
    }

    VertexDataTypeCustom(const unsigned int& _data_type, const unsigned int& size);
protected:
    unsigned int _data_type;
    unsigned int _size;
};

class VertexAttributePos3F : public IVertexAttribute, public StaticSingleton<VertexAttributePos3F> {
public:
    inline unsigned int GetSize() override {return ((unsigned int)VertexDataTypeFloat::GetInstance().GetSize()) * ((unsigned int)3);}
    inline unsigned int GetElementsNum() override { return 3; }
    inline IVertexDataType* GetDataType() override { return dynamic_cast<MR::IVertexDataType*>(&VertexDataTypeFloat::GetInstance()); }
    inline unsigned int GetShaderIndex() override { return IVertexAttribute::Position; }

    inline bool Equal(IVertexAttribute* va) override {
        if((VertexDataTypeFloat::GetInstance().GetSize() * 3) != va->GetSize()) return false;
        if(3 != va->GetElementsNum()) return false;
        if(IVertexAttribute::Position != va->GetShaderIndex()) return false;

        return VertexDataTypeFloat::GetInstance().Equal(va->GetDataType());
    }
};

class VertexAttributeCustom : public IVertexAttribute {
public:
    inline unsigned int GetSize() override { return _size; }
    inline unsigned int GetElementsNum() override { return _el_num; }
    inline IVertexDataType* GetDataType() override { return _data_type; }
    inline unsigned int GetShaderIndex() override { return _shaderIndex; }

    inline bool Equal(IVertexAttribute* va) override {
        if(_size != va->GetSize()) return false;
        if(_el_num != va->GetElementsNum()) return false;
        if(_shaderIndex != va->GetShaderIndex()) return false;

        return _data_type->Equal(va->GetDataType());
    }

    VertexAttributeCustom(const unsigned int& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex);
protected:
    unsigned int _el_num;
    IVertexDataType* _data_type;
    unsigned int _size;
    unsigned int _shaderIndex;
};

class VertexFormatCustom : public IVertexFormat {
public:
    inline unsigned int GetSize() override { return _size; }
    void AddVertexAttribute(IVertexAttribute* a) override;
    bool Bind() override;
    void UnBind() override;
    bool Equal(IVertexFormat* vf) override;

    VertexFormatCustom();
    ~VertexFormatCustom();

    inline TStaticArray<IVertexAttribute*> _GetAttributes() override { return TStaticArray<IVertexAttribute*>(&_attribs[0], _attribs.size(), false); }
    inline TStaticArray<uint64_t> _GetOffsets() override { return TStaticArray<uint64_t>(&_pointers[0], _pointers.size(), false); }
protected:
    std::vector<IVertexAttribute*> _attribs;
    std::vector<uint64_t> _pointers;
    uint64_t _nextPtr;
    unsigned int _size;
};

class VertexFormatCustomFixed : public IVertexFormat {
public:
    inline unsigned int GetSize() override { return _size; }
    inline void AddVertexAttribute(IVertexAttribute* a) override { SetVertexAttribute(a, _nextIndex); ++_nextIndex; }
    bool Bind() override;
    void UnBind() override;
    bool Equal(IVertexFormat* vf) override;

    /// call this after creating new object of this type
    /// resets all data
    void SetAttributesNum(const size_t& num);

    VertexFormatCustomFixed();
    ~VertexFormatCustomFixed();

    inline TStaticArray<IVertexAttribute*> _GetAttributes() override { return TStaticArray<IVertexAttribute*>(&_attribs[0], _attribsNum, false); }
    inline TStaticArray<uint64_t> _GetOffsets() override { return TStaticArray<uint64_t>(&_pointers[0], _attribsNum, false); }
protected:
    void _RecalcSize(); //SetVertexAttribute calls it automatically
    void SetVertexAttribute(IVertexAttribute* a, const size_t& index);

    IVertexAttribute** _attribs;
    uint64_t* _pointers;
    unsigned int _size;
    size_t _nextIndex;
    size_t _attribsNum;
};

class IndexFormatCustom : public IIndexFormat {
public:
    inline unsigned int GetSize() override { return _dataType->GetSize(); }
    inline void SetDataType(IVertexDataType* dataType) override { _dataType = dataType; }
    inline IVertexDataType* GetDataType() override { return _dataType; }
    bool Bind() override;
    void UnBind() override;
    bool Equal(IIndexFormat* ifo) override;

    IndexFormatCustom(IVertexDataType* dataType);
    ~IndexFormatCustom();
protected:
    IVertexDataType* _dataType;
};

IVertexFormat* VertexFormatGetBinded();
void VertexFormatUnBind();

IIndexFormat* IndexFormatGetBinded();
void IndexFormatUnBind();

}

#endif // _MR_GEOMETRY_FORMATS_H_
