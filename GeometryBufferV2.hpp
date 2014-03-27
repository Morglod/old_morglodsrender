#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "Types.hpp"
#include <stack>

namespace MR {

class IVertexDataType {
public:
    virtual unsigned char Size() = 0; //one element of this data type size in bytes
    virtual unsigned int GPUDataType() = 0; //returns opengl data type
};

class VertexDataTypeInt : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(int); }
    inline unsigned int GPUDataType() override { return 0x1404; }

    static IVertexDataType* Instance();
};

class VertexDataTypeUInt : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(unsigned int); }
    inline unsigned int GPUDataType() override { return 0x1405; }

    static IVertexDataType* Instance();
};

class VertexDataTypeFloat : public IVertexDataType {
public:
    inline unsigned char Size() override { return sizeof(float); }
    inline unsigned int GPUDataType() override { return 0x1406; }

    static IVertexDataType* Instance();
};

class VertexDataTypeCustom : public IVertexDataType {
public:
    inline unsigned char Size() override {return _size;}
    inline unsigned int GPUDataType() override {return _gpu_type;}

    VertexDataTypeCustom(const unsigned int& gpu_type, const unsigned char& size);
protected:
    unsigned int _gpu_type;
    unsigned char _size;
};

class IVertexAttribute {
public:
    virtual unsigned int Size() = 0; //one attrib size in bytes
    virtual unsigned char ElementsNum() = 0; //num of elements used in attribute
    virtual IVertexDataType* DataType() = 0; //returns it's data type
};

class VertexAttributePos3F : public IVertexAttribute {
public:
    inline unsigned int Size() override {return ((unsigned int)VertexDataTypeFloat::Instance()->Size()) * ((unsigned int)3);}
    inline unsigned char ElementsNum() override { return 3; }
    inline IVertexDataType* DataType() override {return VertexDataTypeFloat::Instance();}
};

class VertexAttributeCustom : public IVertexAttribute {
public:
    inline unsigned int Size() override { return _size; }
    inline unsigned char ElementsNum() override { return _el_num; }
    inline IVertexDataType* DataType() override { return _data_type; }

    VertexAttributeCustom(const unsigned char& elementsNum, IVertexDataType* dataType);
protected:
    unsigned char _el_num;
    IVertexDataType* _data_type;
    unsigned int _size;
};

class IVertexFormat {
public:
    virtual unsigned int Size() = 0; //one vertex size in bytes
    virtual MR::Array<IVertexAttribute*> VertexAttributes() = 0;
    virtual void SetVertexAttributes(const MR::Array<IVertexAttribute*>& va) = 0; //call it after any of vertex attributes changed
};

class VertexFormatCustom : public IVertexFormat {
public:
    inline unsigned int Size() override { return _size; }
    inline MR::Array<IVertexAttribute*> VertexAttributes() override { return _va; }
    void SetVertexAttributes(const MR::Array<IVertexAttribute*>& va) override; //call it after any of vertex attributes changed
    VertexFormatCustom(const MR::Array<IVertexAttribute*>& va);
protected:
    MR::Array<IVertexAttribute*> _va;
    unsigned int _size;
};

class IGLBuffer {
public:
    enum UsageFlags {
        Stream = 0x88E0,
        Static = 0x88E4,
        Dynamic = 0x88E8,

        //Stream/Static/Dynamic + one of this
        Draw = 0,
        Read = 1,
        Copy = 2
    };

    enum AccessFlags {
        ReadOnly = 0x88B8,
        WriteOnly = 0x88B9,
        ReadWrite = 0x88BA
    };

    virtual bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag = ReadOnly) = 0;
    virtual void Release() = 0;
};

class VertexBuffer : public Super, public IGLBuffer {
public:
    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag) override;
    void Release() override;

    VertexBuffer();
    virtual ~VertexBuffer();

protected:
    uint64_t _resident_ptr;
    int _buffer_size;
    unsigned int _usage;
    unsigned int _accessFlag;
    unsigned int _handle;
};

}

#endif // _MR_GEOMETRY_BUFFER_H_
