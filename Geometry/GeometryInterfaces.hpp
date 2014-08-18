#pragma once

#ifndef _MR_GEOMETRY_INTERFACES_
#define _MR_GEOMETRY_INTERFACES_

#include "../Types.hpp"
#include "../Utils/Containers.hpp"
#include "../Shaders/ShaderConfig.hpp"

namespace MR {

class GPUBuffer;
class IRenderSystem;
class IGeometryStream;

class IVertexDataType : public Comparable<IVertexDataType*> {
public:
    virtual unsigned char Size() = 0; //one element of this data type size in bytes
    virtual unsigned int GPUDataType() = 0; //returns opengl data type
};

class IVertexAttribute : public Comparable<IVertexAttribute*> {
public:
    enum ShaderIndex {
        SI_Position = MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION,
        SI_Normal = MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION,
        SI_Color = MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION,
        SI_TexCoord = MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION
    };

    virtual uint64_t Size() = 0; //one attrib size in bytes
    virtual unsigned char ElementsNum() = 0; //num of elements used in attribute
    virtual IVertexDataType* DataType() = 0; //returns it's data type
    virtual unsigned int ShaderIndex() = 0;
};

class IVertexFormat : public Comparable<IVertexFormat*> {
public:
    virtual size_t Size() = 0; //one vertex size in bytes
    virtual void AddVertexAttribute(IVertexAttribute* a) = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;

    virtual StaticArray<IVertexAttribute*> _Attributes() = 0;
    virtual StaticArray<uint64_t> _Offsets() = 0; //offsets of each attributes from starting point of vertex in bytes
};

class IInstancedDataFormat : public Comparable<IInstancedDataFormat*> {
public:
    virtual size_t Size() = 0; //one instance data size in byte
    virtual void AddAttribute(IVertexAttribute* a) = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;

    virtual StaticArray<IVertexAttribute*> _Attributes() = 0;
    virtual StaticArray<uint64_t> _Offsets() = 0;
};

class IIndexFormat : public Comparable<IIndexFormat*> {
public:
    virtual size_t Size() = 0; //one index size in bytes
    virtual void SetDataType(IVertexDataType* dataType) = 0;
    virtual IVertexDataType* GetDataType() = 0;
    virtual bool Bind() = 0;
    virtual void UnBind() = 0;
};

//It's only storage for data, that will be buffered. After buffering it may be deleted
class IVertexData {
public:
    virtual unsigned int GetDrawMode() = 0;
    virtual size_t GetSize() = 0; //size of all data for vertexes
    virtual void* GetData() = 0; //may be nullptr if data is deleted
    virtual IVertexFormat* GetFormat() = 0;
    virtual ~IVertexData() {}
};

//It's only storage for data, that will be buffered. After buffering it may be deleted
class IIndexData {
public:
    virtual unsigned int GetDrawMode() = 0;
    virtual size_t GetSize() = 0; //size of all data for indecies
    virtual void* GetData() = 0; //may be nullptr if data is deleted
    virtual IIndexFormat* GetFormat() = 0;
    virtual ~IIndexData() {}
};

class IBuffer {
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

    enum StorageBits {
        DynamicBit = 0x0100,
        ReadBit = 0x0001,
        WriteBit = 0x0002,
        PersistentBit = 0x00000040,
        CoherentBit = 0x00000080,
        ClientStorageBit = 0x0200
    };

    //stream->format should be binded
    virtual void BindToStream(IGeometryStream* stream, const unsigned int& offset) = 0;

    virtual bool Buffer(void* data, const unsigned int& size, const unsigned int& usage) = 0;
    virtual bool BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) = 0;
    virtual bool Update(void* data, const unsigned int& size, const unsigned int& offset) = 0;
    virtual bool GetBuffered(void* data, const unsigned int& offset, const unsigned int& size) = 0;
    virtual void Release() = 0;

    virtual size_t GetAllocatedSize() = 0;
    virtual size_t GetUsedSize() = 0;
    virtual size_t GetFreeSize() = 0;
    virtual unsigned int GetNextFreeOffset() = 0;

    virtual bool _CopyTo(const unsigned int& dstHandle, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) = 0;

    virtual unsigned int GetGPUHandle() = 0;
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

    virtual bool SetVertexBuffer(GPUBuffer* buf) = 0;
    virtual GPUBuffer* GetVertexBuffer() = 0;

    virtual bool SetIndexBuffer(GPUBuffer* buf) = 0;
    virtual GPUBuffer* GetIndexBuffer() = 0;

    virtual void Release() = 0;

    virtual void SetDrawMode(const unsigned int& dm) = 0;
    virtual unsigned int GetDrawMode() = 0;

    virtual void SetFormat(IVertexFormat* f, IIndexFormat* fi) = 0;
    virtual IVertexFormat* GetVertexFormat() = 0;
    virtual IIndexFormat* GetIndexFormat() = 0;

    virtual unsigned int GetVAO() = 0; //if zero, don't use vao

    virtual ~IGeometryBuffer() {}
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

    //Eg for indirect buffer binding
    virtual void BeginDraw() = 0;
    virtual void EndDraw() = 0;

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

/*
class IInstancedGeometry : public IGeometry {
public:
    virtual GPUBuffer* GetInstancedBuffer() = 0;
    virtual void SetInstancedBuffer(GPUBuffer* buf) = 0;

    virtual IInstancedDataFormat* GetInstancedDataFormat() = 0;
    virtual void SetInstancedDataFormat(IInstancedDataFormat* format) = 0;

    virtual ~IInstancedGeometry() {}
};
*/

class IGeometryStream {
public:
    virtual unsigned int GetGPUId() = 0;
    virtual IVertexFormat* GetBindedVertexFormat() = 0;

    virtual void BindVertexFormat(IVertexFormat* format) = 0;

    virtual ~IGeometryStream() {}
};

}

#endif
