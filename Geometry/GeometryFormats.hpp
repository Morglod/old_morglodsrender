#pragma once

#ifndef _MR_GEOMETRY_FORMATS_H_
#define _MR_GEOMETRY_FORMATS_H_

#include "../Types.hpp"
#include "../Config.hpp"
#include "GeometryInterfaces.hpp"
#include "Singleton.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace mr {

class VertexFormatCustom;
class VertexFormatCustomFixed;

class VertexDataTypeInt : public IVertexDataType, public mu::StaticSingleton<VertexDataTypeInt> {
public:
    inline unsigned int GetSize() const override { return sizeof(int); }
    inline unsigned int GetDataType() const override { return 0x1404; }

    inline bool Equal(IVertexDataType* dt) const override {
        if(sizeof(int) != dt->GetSize()) return false;
        if(0x1404 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeUInt : public IVertexDataType, public mu::StaticSingleton<VertexDataTypeUInt> {
public:
    inline unsigned int GetSize() const override { return sizeof(unsigned int); }
    inline unsigned int GetDataType() const override { return 0x1405; }

    inline bool Equal(IVertexDataType* dt) const override {
        if(sizeof(unsigned int) != dt->GetSize()) return false;
        if(0x1405 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeFloat : public IVertexDataType, public mu::StaticSingleton<VertexDataTypeFloat> {
public:
    inline unsigned int GetSize() const override { return sizeof(float); }
    inline unsigned int GetDataType() const override { return 0x1406; }

    inline bool Equal(IVertexDataType* dt) const override {
        if(sizeof(float) != dt->GetSize()) return false;
        if(0x1406 != dt->GetDataType()) return false;
        return true;
    }
};

class VertexDataTypeCustom : public IVertexDataType {
public:
    inline unsigned int GetSize() const override {return _size;}
    inline unsigned int GetDataType() const override {return _data_type;}

    inline bool Equal(IVertexDataType* dt) const override {
        if(_size != dt->GetSize()) return false;
        if(_data_type != dt->GetDataType()) return false;
        return true;
    }

    IVertexDataType* Cache();

    VertexDataTypeCustom();
    VertexDataTypeCustom(VertexDataTypeCustom const& cpy);
    VertexDataTypeCustom(const unsigned int& data_type, const unsigned int& size);
protected:
    unsigned int _data_type;
    unsigned int _size;
};

class VertexAttributePos3F : public IVertexAttribute, public mu::StaticSingleton<VertexAttributePos3F> {
public:
    inline unsigned int GetSize() const override {return ((unsigned int)VertexDataTypeFloat::GetInstance().GetSize()) * ((unsigned int)3);}
    inline unsigned int GetElementsNum() const override { return 3; }
    inline IVertexDataType* GetDataType() const override { return dynamic_cast<mr::IVertexDataType*>(&VertexDataTypeFloat::GetInstance()); }
    inline unsigned int GetShaderIndex() const override { return IVertexAttribute::Position; }

    inline unsigned int GetDivisor() const { return _divisor; }
    inline void SetDivisor(unsigned int const& divisor) override { _divisor = divisor; }

    inline bool Equal(IVertexAttribute* va) const override {
        if((VertexDataTypeFloat::GetInstance().GetSize() * 3) != va->GetSize()) return false;
        if(3 != va->GetElementsNum()) return false;
        if(IVertexAttribute::Position != va->GetShaderIndex()) return false;
        if(_divisor != va->GetDivisor()) return false;

        return VertexDataTypeFloat::GetInstance().Equal(va->GetDataType());
    }
protected:
    unsigned int _divisor = 0;
};

class VertexAttributeCustom : public IVertexAttribute {
public:
    inline unsigned int GetSize() const override { return _size; }
    inline unsigned int GetElementsNum() const override { return _el_num; }
    inline IVertexDataType* GetDataType() const override { return _data_type; }
    inline unsigned int GetShaderIndex() const override { return _shaderIndex; }

    inline unsigned int GetDivisor() const override { return _divisor; }
    inline void SetDivisor(unsigned int const& divisor) override { _divisor = divisor; }

    inline bool Equal(IVertexAttribute* va) const override {
        if(_size != va->GetSize()) return false;
        if(_el_num != va->GetElementsNum()) return false;
        if(_shaderIndex != va->GetShaderIndex()) return false;
        if(_divisor != va->GetDivisor()) return false;
        return _data_type->Equal(va->GetDataType());
    }

    IVertexAttribute* Cache();

    VertexAttributeCustom();
    VertexAttributeCustom(VertexAttributeCustom const& cpy);
    VertexAttributeCustom(unsigned int const& elementsNum, IVertexDataType* dataType, unsigned int const& shaderIndex, unsigned const& divisor = 0);
protected:
    unsigned int _el_num;
    IVertexDataType* _data_type;
    unsigned int _size;
    unsigned int _shaderIndex;
    unsigned int _divisor;
};

class VertexFormatCustom : public IVertexFormat {
public:
    inline unsigned int GetSize() const override { return _size; }
    void AddVertexAttribute(IVertexAttribute* a) override;
    bool Bind() const override;
    void UnBind() const override;
    bool Equal(IVertexFormat* vf) const override;

    IVertexFormat* Cache();

    VertexFormatCustom();
    VertexFormatCustom(VertexFormatCustom const& cpy);
    virtual ~VertexFormatCustom();

    inline TArrayRef<IVertexAttribute*> GetAttributes() override { return TArrayRef<IVertexAttribute*>(&_attribs[0], _attribs.size()); }
    inline TArrayRef<uint64_t> GetOffsets() override { return TArrayRef<uint64_t>(&_pointers[0], _pointers.size()); }
protected:
    std::vector<IVertexAttribute*> _attribs;
    std::vector<uint64_t> _pointers;
    uint64_t _nextPtr;
    unsigned int _size;
};

class VertexFormatCustomFixed : public IVertexFormat {
public:
    inline unsigned int GetSize() const override { return _size; }
    inline void AddVertexAttribute(IVertexAttribute* a) override { SetVertexAttribute(a, _nextIndex); ++_nextIndex; }
    bool Bind() const override;
    void UnBind() const override;
    bool Equal(IVertexFormat* vf) const override;

    /// call this after creating new object of this type
    /// resets all data
    void SetAttributesNum(const size_t& num);

    IVertexFormat* Cache();

    VertexFormatCustomFixed();
    VertexFormatCustomFixed(VertexFormatCustomFixed const& cpy);
    virtual ~VertexFormatCustomFixed();

    inline TArrayRef<IVertexAttribute*> GetAttributes() override { return TArrayRef<IVertexAttribute*>(&_attribs[0], _attribsNum); }
    inline TArrayRef<uint64_t> GetOffsets() override { return TArrayRef<uint64_t>(&_pointers[0], _attribsNum); }
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
    inline unsigned int GetSize() const override { return _dataType->GetSize(); }
    inline void SetDataType(IVertexDataType* dataType) override { _dataType = dataType; }
    inline IVertexDataType* GetDataType() const override { return _dataType; }
    bool Bind() const override;
    void UnBind() const override;
    bool Equal(IIndexFormat* ifo) const override;

    IIndexFormat* Cache();

    IndexFormatCustom();
    IndexFormatCustom(IndexFormatCustom const& cpy);
    IndexFormatCustom(IVertexDataType* dataType);
    virtual ~IndexFormatCustom();
protected:
    IVertexDataType* _dataType;
};

IVertexFormat* VertexFormatGetBinded();
void VertexFormatUnBind();

IIndexFormat* IndexFormatGetBinded();
void IndexFormatUnBind();

}

#endif // _MR_GEOMETRY_FORMATS_H_
