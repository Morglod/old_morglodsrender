#pragma once

#ifndef _MR_GEOMETRY_FORMATS_H_
#define _MR_GEOMETRY_FORMATS_H_

#include "../Types.hpp"
#include "../Config.hpp"
#include "GeometryInterfaces.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

namespace MR {

class IRenderSystem;
class Geometry;
class GeometryBuffer;

class VertexFormatCustom;
class VertexFormatCustomFixed;

class VertexDataTypeInt : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(int); }
    inline unsigned int GPUDataType() override { return 0x1404; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(int) != dt->Size()) return false;
        if(0x1404 != dt->GPUDataType()) return false;
        return true;
    }

    static IVertexDataType* Instance();
};

class VertexDataTypeUInt : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(unsigned int); }
    inline unsigned int GPUDataType() override { return 0x1405; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(unsigned int) != dt->Size()) return false;
        if(0x1405 != dt->GPUDataType()) return false;
        return true;
    }

    static IVertexDataType* Instance();
};

class VertexDataTypeFloat : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(float); }
    inline unsigned int GPUDataType() override { return 0x1406; }

    inline bool Equal(IVertexDataType* dt) override {
        if(sizeof(float) != dt->Size()) return false;
        if(0x1406 != dt->GPUDataType()) return false;
        return true;
    }

    static IVertexDataType* Instance();
};

class VertexDataTypeCustom : public IVertexDataType {
public:
    inline unsigned char Size() override {return _size;}
    inline unsigned int GPUDataType() override {return _gpu_type;}

    inline bool Equal(IVertexDataType* dt) override {
        if(_size != dt->Size()) return false;
        if(_gpu_type != dt->GPUDataType()) return false;
        return true;
    }

    VertexDataTypeCustom(const unsigned int& gpu_type, const unsigned char& size);
protected:
    unsigned int _gpu_type;
    unsigned char _size;
};

class VertexAttributePos3F : public IVertexAttribute {
public:
    inline uint64_t Size() override {return ((uint64_t)VertexDataTypeFloat::Instance()->Size()) * ((uint64_t)3);}
    inline unsigned char ElementsNum() override { return 3; }
    inline IVertexDataType* DataType() override { return VertexDataTypeFloat::Instance();}
    inline unsigned int ShaderIndex() override { return IVertexAttribute::SI_Position; }

    inline bool Equal(IVertexAttribute* va) override {
        if((((uint64_t)VertexDataTypeFloat::Instance()->Size()) * ((uint64_t)3)) != va->Size()) return false;
        if(3 != va->ElementsNum()) return false;
        if(IVertexAttribute::SI_Position != va->ShaderIndex()) return false;

        return VertexDataTypeFloat::Instance()->Equal(va->DataType());
    }
};

class VertexAttributeCustom : public IVertexAttribute {
public:
    inline uint64_t Size() override { return _size; }
    inline unsigned char ElementsNum() override { return _el_num; }
    inline IVertexDataType* DataType() override { return _data_type; }
    inline unsigned int ShaderIndex() override { return _shaderIndex; }

    inline bool Equal(IVertexAttribute* va) override {
        if(_size != va->Size()) return false;
        if(_el_num != va->ElementsNum()) return false;
        if(_shaderIndex != va->ShaderIndex()) return false;

        return _data_type->Equal(va->DataType());
    }

    VertexAttributeCustom(const unsigned char& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex);
protected:
    unsigned char _el_num;
    IVertexDataType* _data_type;
    uint64_t _size;
    unsigned int _shaderIndex;
};

class VertexFormatCustom : public IVertexFormat {
public:
    inline size_t Size() override { return _size; }
    void AddVertexAttribute(IVertexAttribute* a) override;
    bool Bind() override;
    void UnBind() override;
    bool Equal(IVertexFormat* vf) override;

    VertexFormatCustom();
    ~VertexFormatCustom();
protected:
    inline size_t _Attributes(IVertexAttribute*** dst) override { *dst = &_attribs[0]; return _attribs.size(); }
    inline size_t _Offsets(uint64_t** dst) override { *dst = &_pointers[0]; return _pointers.size(); }

    std::vector<IVertexAttribute*> _attribs;
    std::vector<uint64_t> _pointers;
    uint64_t _nextPtr;
    size_t _size;
};

class VertexFormatCustomFixed : public IVertexFormat {
public:
    inline size_t Size() override { return _size; }
    inline void AddVertexAttribute(IVertexAttribute* a) override { SetVertexAttribute(a, _nextIndex); ++_nextIndex; }
    bool Bind() override;
    void UnBind() override;
    bool Equal(IVertexFormat* vf) override;

    /// call this after creating new object of this type
    /// resets all data
    void SetAttributesNum(const size_t& num);
    void RecalcSize(); //SetVertexAttribute calls it automatically

    VertexFormatCustomFixed();
    ~VertexFormatCustomFixed();
protected:
    inline size_t _Attributes(IVertexAttribute*** dst) override { *dst = _attribs; return _attribsNum; }
    inline size_t _Offsets(uint64_t** dst) override { *dst = _pointers; return _attribsNum; }
    void SetVertexAttribute(IVertexAttribute* a, const size_t& index);

    IVertexAttribute** _attribs;
    uint64_t* _pointers;
    size_t _size;
    size_t _nextIndex;
    size_t _attribsNum;
};

class IndexFormatCustom : public IIndexFormat {
public:
    inline size_t Size() override { return _dataType->Size(); }
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

}

#endif // _MR_GEOMETRY_FORMATS_H_
