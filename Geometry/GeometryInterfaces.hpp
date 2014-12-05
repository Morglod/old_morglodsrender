#pragma once

#ifndef _MR_GEOMETRY_INTERFACES_
#define _MR_GEOMETRY_INTERFACES_

#include "../Types.hpp"
#include "../Utils/Containers.hpp"
#include "../Shaders/ShaderConfig.hpp"
#include "../Buffers/BuffersInterfaces.hpp"

namespace mr {

class IVertexDataType : public Comparable<IVertexDataType*> {
public:
    enum DefaultDataType {
        Int = 0x1404,
        UInt = 0x1405,
        Float = 0x1406
    };

    virtual unsigned int GetSize() = 0; //one element of this data type size in bytes
    virtual unsigned int GetDataType() = 0; //opengl data type

    virtual ~IVertexDataType() {}
};

class IVertexAttribute : public Comparable<IVertexAttribute*> {
public:
    enum DefaultShaderIndex {
        Position = MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION,
        Normal = MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION,
        Color = MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION,
        TexCoord = MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION
    };

    virtual unsigned int GetSize() = 0; //one attrib size in bytes
    virtual unsigned int GetElementsNum() = 0; //num of elements used in attribute
    virtual IVertexDataType* GetDataType() = 0;
    virtual unsigned int GetShaderIndex() = 0;

    virtual ~IVertexAttribute() {}
};

class IVertexFormat : public Comparable<IVertexFormat*> {
public:
    virtual unsigned int GetSize() = 0; //one vertex size in bytes
    virtual void AddVertexAttribute(IVertexAttribute* a) = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;

    virtual TStaticArray<IVertexAttribute*> _GetAttributes() = 0;
    virtual TStaticArray<uint64_t> _GetOffsets() = 0; //offsets of each attributes from starting point of vertex in bytes

    virtual ~IVertexFormat() {}
};

class IInstancedDataFormat : public Comparable<IInstancedDataFormat*> {
public:
    virtual unsigned int GetSize() = 0; //one instance data size in byte
    virtual void AddAttribute(IVertexAttribute* a) = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;

    virtual TStaticArray<IVertexAttribute*> _GetAttributes() = 0;
    virtual TStaticArray<uint64_t> _GetOffsets() = 0;

    virtual ~IInstancedDataFormat() {}
};

class IIndexFormat : public Comparable<IIndexFormat*> {
public:
    virtual unsigned int GetSize() = 0; //one index size in bytes
    virtual void SetDataType(IVertexDataType* dataType) = 0;
    virtual IVertexDataType* GetDataType() = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;

    virtual ~IIndexFormat() {}
};

//When buffered is true, data may be deleted
class VertexData {
public:
    unsigned int drawMode = 0;
    IVertexFormat* vertexFormat = nullptr;
    size_t dataSize = 0;
    void* data = nullptr;
    volatile bool buffered = false;
};

//When buffered is true, data may be deleted
class IndexData {
public:
    unsigned int drawMode = 0;
    IIndexFormat* indexFormat = nullptr;
    size_t dataSize = 0;
    void* data = nullptr;
    volatile bool buffered = false;
};

class IGeometryBuffer {
public:
    enum DrawMode {
        Points = 0,
        Lines = 1,
        Triangles,
        Quads
    };

    struct CreationParams {
    public:
        IGPUBuffer* vb = nullptr,* ib = nullptr;
        IVertexFormat* fv = nullptr;
        IIndexFormat* fi = nullptr;
        IGeometryBuffer::DrawMode drawMode = DrawMode::Triangles;

        CreationParams() {}
        CreationParams(IGPUBuffer* vb_, IGPUBuffer* ib_, IVertexFormat* fv_, IIndexFormat* fi_, IGeometryBuffer::DrawMode drawMode_) :
            vb(vb_), ib(ib_), fv(fv_), fi(fi_), drawMode(drawMode_) {}
    };

    virtual bool Create(CreationParams const& params) = 0;

    virtual bool SetVertexBuffer(IGPUBuffer* buf) = 0;
    virtual IGPUBuffer* GetVertexBuffer() = 0;

    virtual bool SetIndexBuffer(IGPUBuffer* buf) = 0;
    virtual IGPUBuffer* GetIndexBuffer() = 0;

    virtual bool GetVertexBuffer_NVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) = 0;
    virtual bool GetIndexBuffer_NVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) = 0;

    virtual bool Bind(bool useIndexBuffer) = 0;

    virtual bool Good() = 0;

    virtual void SetDrawMode(const DrawMode& dm) = 0;
    virtual DrawMode GetDrawMode() = 0;

    virtual void SetFormat(IVertexFormat* f, IIndexFormat* fi) = 0;
    virtual IVertexFormat* GetVertexFormat() = 0;
    virtual IIndexFormat* GetIndexFormat() = 0;
};

class IGeometryDrawParams {
public:
    typedef struct {
        unsigned int count;
        unsigned int instanceCount;
        unsigned int first;
        unsigned int baseInstance;
    } DrawArraysIndirectCmd;

    typedef struct {
        unsigned int count;
        unsigned int primCount;
        unsigned int firstIndex;
        unsigned int baseVertex;
        unsigned int baseInstance;
    } DrawElementsIndirectCmd;

    virtual void SetIndexStart(const unsigned int& i) = 0;
    virtual void SetVertexStart(const unsigned int& v) = 0;
    virtual void SetIndexCount(const unsigned int& i) = 0;
    virtual void SetVertexCount(const unsigned int& i) = 0;

    virtual unsigned int GetIndexStart() = 0;
    virtual unsigned int GetVertexStart() = 0;
    virtual unsigned int GetIndexCount() = 0;
    virtual unsigned int GetVertexCount() = 0;

    virtual void* GetIndirectPtr() = 0;

    virtual void SetUseIndexBuffer(const bool& state) = 0;
    virtual bool GetUseIndexBuffer() = 0;

    virtual ~IGeometryDrawParams() {}
};

typedef std::shared_ptr<IGeometryDrawParams> IGeometryDrawParamsPtr;

class IGeometry {
public:
    virtual IGeometryBuffer* GetGeometryBuffer() = 0;
    virtual void SetGeometryBuffer(IGeometryBuffer* buffer) = 0;

    virtual IGeometryDrawParamsPtr GetDrawParams() = 0;
    virtual void SetDrawParams(IGeometryDrawParamsPtr params) = 0;

    virtual void Draw() = 0;
    virtual ~IGeometry() {}
};

}

#endif
