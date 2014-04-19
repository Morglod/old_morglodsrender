#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "Types.hpp"
#include "Config.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

namespace MR {

class IRenderSystem;
class Geometry;
class GeometryBuffer;

class IVertexDataType : public Comparable<IVertexDataType*> {
public:
    virtual unsigned char Size() = 0; //one element of this data type size in bytes
    virtual unsigned int GPUDataType() = 0; //returns opengl data type
};

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

class IVertexAttribute : public Comparable<IVertexAttribute*> {
public:
    enum ShaderIndex {
        ShaderIndex_Position = MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION,
        ShaderIndex_Normal = MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION,
        ShaderIndex_Color = MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION,
        ShaderIndex_TexCoord = MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION
    };

    virtual uint64_t Size() = 0; //one attrib size in bytes
    virtual unsigned char ElementsNum() = 0; //num of elements used in attribute
    virtual IVertexDataType* DataType() = 0; //returns it's data type
    virtual unsigned int ShaderIndex() = 0;
};

class VertexAttributePos3F : public IVertexAttribute {
public:
    inline uint64_t Size() override {return ((uint64_t)VertexDataTypeFloat::Instance()->Size()) * ((uint64_t)3);}
    inline unsigned char ElementsNum() override { return 3; }
    inline IVertexDataType* DataType() override { return VertexDataTypeFloat::Instance();}
    inline unsigned int ShaderIndex() override { return IVertexAttribute::ShaderIndex_Position; }

    inline bool Equal(IVertexAttribute* va) override {
        if((((uint64_t)VertexDataTypeFloat::Instance()->Size()) * ((uint64_t)3)) != va->Size()) return false;
        if(3 != va->ElementsNum()) return false;
        if(IVertexAttribute::ShaderIndex_Position != va->ShaderIndex()) return false;

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

class IVertexFormat : public Comparable<IVertexFormat*> {
    friend class GeometryBuffer;
public:
    virtual size_t Size() = 0; //one vertex size in bytes
    virtual void AddVertexAttribute(IVertexAttribute* a) = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;
protected:
    virtual std::vector<IVertexAttribute*>* _Attributes() = 0;
    virtual std::vector<uint64_t>* _Offsets() = 0; //offsets of each attributes from starting point of vertex in bytes
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
    inline std::vector<IVertexAttribute*>* _Attributes() override { return &_attribs; }
    inline std::vector<uint64_t>* _Offsets() override { return &_pointers; }

    std::vector<IVertexAttribute*> _attribs;
    std::vector<uint64_t> _pointers;
    uint64_t _nextPtr;
    size_t _size;
};

class IIndexFormat {
    friend class GeometryBuffer;
public:
    virtual size_t Size() = 0; //one index size in bytes
    virtual void SetDataType(IVertexDataType* dataType) = 0;
    virtual IVertexDataType* GetDataType() = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;
};

class IndexFormatCustom : public IIndexFormat {
public:
    inline size_t Size() override { return _dataType->Size(); }
    inline void SetDataType(IVertexDataType* dataType) override { _dataType = dataType; }
    inline IVertexDataType* GetDataType() override { return _dataType; }
    bool Bind() override;
    void UnBind() override;

    IndexFormatCustom(IVertexDataType* dataType);
    ~IndexFormatCustom();
protected:
    IVertexDataType* _dataType;
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

class VertexBuffer : public Object, public IGLBuffer {
    friend class GeometryBuffer;
public:
    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag) override;
    void Release() override;
    inline void SetNum(const unsigned int & num) { _num = num; }
    inline unsigned int GetNum() { return _num; }

    VertexBuffer();
    virtual ~VertexBuffer();

protected:
    uint64_t _resident_ptr;
    int _buffer_size;
    unsigned int _usage;
    unsigned int _accessFlag;
    unsigned int _handle;
    unsigned int _num;
};

class IndexBuffer : public Object, public IGLBuffer {
    friend class GeometryBuffer;
public:
    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag) override;
    inline void SetNum(const unsigned int & num) { _num = num; }
    inline unsigned int GetNum() { return _num; }
    void Release() override;

    IndexBuffer();
    virtual ~IndexBuffer();

protected:
    uint64_t _resident_ptr;
    int _buffer_size;
    unsigned int _usage;
    unsigned int _accessFlag;
    unsigned int _handle;
    unsigned int _num;
};

class IndirectDrawBuffer : public Object, public IGLBuffer {
    friend class Geometry;
public:
    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag) override;
    void Release() override;
    inline unsigned int GetHandle() { return _handle; }

    IndirectDrawBuffer();
    virtual ~IndirectDrawBuffer();

protected:
    uint64_t _resident_ptr;
    int _buffer_size;
    unsigned int _usage;
    unsigned int _accessFlag;
    unsigned int _handle;
};

class IGeometryBuffer {
public:
    enum DrawModes {
        Draw_Points = 0,
        Draw_Lines = 1,
        Draw_LineLoop = 2,
        Draw_LineStrip = 3,
        Draw_Triangles = 4,
        Draw_TriangleStrip = 5,
        Draw_TriangleFan = 6,
        Draw_Quads = 7,
        Draw_QuadStrip = 8
    };

    virtual bool SetVertexBuffer(VertexBuffer* buf) = 0;
    virtual VertexBuffer* GetVertexBuffer() = 0;

    virtual bool SetIndexBuffer(IndexBuffer* buf) = 0;
    virtual IndexBuffer* GetIndexBuffer() = 0;

    virtual void Draw(IRenderSystem* rc, const unsigned int& start, const unsigned int& end, const int& count) = 0;
    virtual void SetFormat(IVertexFormat* f, IIndexFormat* fi) = 0;
    virtual ~IGeometryBuffer() {}
};

class GeometryBuffer : public Object, public IGeometryBuffer {
    friend class VertexBuffer;
    friend class IndexBuffer;
public:
    bool SetVertexBuffer(VertexBuffer* buf) override;
    inline VertexBuffer* GetVertexBuffer() override { return _vb; }
    bool SetIndexBuffer(IndexBuffer* buf) override;
    inline IndexBuffer* GetIndexBuffer() override { return _ib; }
    void Draw(IRenderSystem* rc, const unsigned int& start, const unsigned int& end, const int& count) override;
    inline void SetFormat(IVertexFormat* f, IIndexFormat* fi) override { _format = f; _iformat = fi; }

    static GeometryBuffer* Plane(const glm::vec3& scale, const glm::vec3 pos, const unsigned int& usage, const unsigned int& drawm);

    GeometryBuffer(VertexBuffer* vb, IndexBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode);
    virtual ~GeometryBuffer();
protected:
    VertexBuffer* _vb;
    IndexBuffer* _ib;
    IVertexFormat* _format;
    IIndexFormat* _iformat;
    unsigned int _vao;
    unsigned int _draw_mode;
};

class IGeometry {
public:
    virtual void SetGeometryBuffer(IGeometryBuffer* buffer) = 0;
    virtual void SetStart(const unsigned int& i) = 0;
    virtual void SetEnd(const unsigned int& i) = 0;
    virtual void SetCount(const int& i) = 0;
    virtual void Draw(IRenderSystem* rc) = 0;
    virtual ~IGeometry() {}
};

class Geometry : public IGeometry, public Object {
public:
    void SetGeometryBuffer(IGeometryBuffer* buffer) override;
    void SetStart(const unsigned int& i) override;
    void SetEnd(const unsigned int& i) override;
    void SetCount(const int& i) override;
    void Draw(IRenderSystem* rc) override;

    Geometry(IGeometryBuffer* buffer, const unsigned int& istart, const unsigned int& iend, const int& icount);
    virtual ~Geometry();

protected:
    void _MakeBuffer(); //make indirect drawing buffer
    IGeometryBuffer* _buffer;
    IndirectDrawBuffer* _draw_buffer; //buffer start, end, count values for indirect drawing
    unsigned int _start, _end;
    int _count;
};

}

#endif // _MR_GEOMETRY_BUFFER_H_
