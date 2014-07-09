#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "../Types.hpp"
#include "../Config.hpp"

#include "../GL/GLObject.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

#include "GeometryFormats.hpp"

namespace MR {

class IRenderSystem;
class Geometry;
class GeometryBuffer;

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

    virtual bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag = ReadOnly) = 0;
    virtual bool Buffer(void* data, const unsigned int& size, const unsigned int& offset) = 0;
    virtual bool BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) = 0;
    virtual bool GetBuffered(void* data, const unsigned int& offset, const unsigned int& size) = 0;
    virtual bool Storage(void* initData, const unsigned int& size, const unsigned int& storageBits) = 0;
    virtual bool Storage(const unsigned int& size, const unsigned int& storageBits) = 0;
    virtual void Release() = 0;

    virtual size_t GetAllocatedSize() = 0;
    virtual size_t GetUsedSize() = 0;
    virtual size_t GetFreeSize() = 0;
    virtual unsigned int GetNextFreeOffset() = 0;

    virtual bool _CopyTo(const unsigned int& dstHandle, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) = 0;
};

class GPUBuffer : public IObject, public IBuffer, public GL::IGLObject {
    friend class GeometryBuffer;
public:
    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag) override;
    bool Buffer(void* data, const unsigned int& size, const unsigned int& offset) override;
    bool BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) override;
    bool GetBuffered(void* data, const unsigned int& offset, const unsigned int& size) override;
    bool Storage(void* initData, const unsigned int& size, const unsigned int& storageBits) override;
    inline bool Storage(const unsigned int& size, const unsigned int& storageBits) override { return this->Storage(0, size, storageBits); }
    void Release() override;
    bool _CopyTo(const unsigned int& dstHandle, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) override;

    inline size_t GetAllocatedSize() override { return _allocated_size; }
    inline size_t GetUsedSize() override { return _used_size; }
    inline size_t GetFreeSize() override { return _free_size; }
    inline unsigned int GetNextFreeOffset() override { return _next_free_offset; }

    inline void SetNum(const unsigned int & num) { _num = num; }
    inline unsigned int GetNum() { return _num; }

    bool IsAvailable() override { return (_handle != 0); }

    GPUBuffer(GL::IContext* ctx, const unsigned int& target);
    virtual ~GPUBuffer();
protected:
    uint64_t _resident_ptr;
    int _buffer_size;
    unsigned int _usage;
    unsigned int _accessFlag;
    unsigned int _handle;
    unsigned int _num;

    unsigned int _target; //gl target / buffer type (vertex, index, etc)

    size_t _allocated_size;
    size_t _used_size;
    size_t _free_size;
    unsigned int _next_free_offset;
};

class VertexBuffer : public GPUBuffer {
public:
    VertexBuffer(GL::IContext* ctx);
    virtual ~VertexBuffer();
};

class IndexBuffer : public GPUBuffer {
public:
    IndexBuffer(GL::IContext* ctx);
    virtual ~IndexBuffer();
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

    virtual void Draw(IRenderSystem* rc, const unsigned int& istart, const unsigned int& vstart, const int& icount, const int& vcount) = 0;
    virtual void SetFormat(IVertexFormat* f, IIndexFormat* fi) = 0;
    virtual ~IGeometryBuffer() {}
};

class GeometryBuffer : public IObject, public IGeometryBuffer, public GL::IGLObject {
    friend class VertexBuffer;
    friend class IndexBuffer;
public:
    bool SetVertexBuffer(GPUBuffer* buf) override;
    inline GPUBuffer* GetVertexBuffer() override { return _vb; }
    bool SetIndexBuffer(GPUBuffer* buf) override;
    inline GPUBuffer* GetIndexBuffer() override { return _ib; }
    void Draw(IRenderSystem* rc, const unsigned int& istart, const unsigned int& vstart, const int& icount, const int& vcount) override;
    inline void SetFormat(IVertexFormat* f, IIndexFormat* fi) override { _format = f; _iformat = fi; }

    bool IsAvailable() override { return true; }
    void Release() override;

    static GeometryBuffer* Plane(GL::IContext* ctx, const glm::vec3& scale, const glm::vec3 pos, const unsigned int& usage, const unsigned int& drawm);

    GeometryBuffer(GL::IContext* ctx, GPUBuffer* vb, GPUBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode);
    virtual ~GeometryBuffer();
protected:
    GPUBuffer* _vb;
    GPUBuffer* _ib;
    IVertexFormat* _format;
    IIndexFormat* _iformat;
    unsigned int _vao;
    unsigned int _draw_mode;
};

class IGeometry {
public:
    virtual void SetGeometryBuffer(IGeometryBuffer* buffer) = 0;
    virtual void SetIStart(const unsigned int& i) = 0;
    virtual void SetVStart(const unsigned int& v) = 0;
    virtual void SetICount(const int& i) = 0;
    virtual void SetVCount(const int& i) = 0;
    virtual void Draw(IRenderSystem* rc) = 0;
    virtual ~IGeometry() {}
};

class Geometry : public IGeometry, public IObject {
public:
    void SetGeometryBuffer(IGeometryBuffer* buffer) override;
    void SetIStart(const unsigned int& i) override;
    void SetVStart(const unsigned int& v) override;
    void SetICount(const int& i) override;
    void SetVCount(const int& i) override;
    void Draw(IRenderSystem* rc) override;

    Geometry(IGeometryBuffer* buffer, const unsigned int& index_start, const unsigned int& vertex_start, const int& ind_count, const int& vert_count);
    virtual ~Geometry();

protected:
    IGeometryBuffer* _buffer;
    unsigned int _istart, _vstart;
    int _icount, _vcount;
};

class GeometryManager {
public:
    GeometryBuffer * PlaceGeometry(  IVertexFormat* vfo, IIndexFormat* ifo,
                                void* vert_data, const size_t& vert_data_size,
                                void* ind_data, const size_t& ind_data_size,
                                const unsigned int& usage, const unsigned int& accessFlag, const unsigned int& drawMode,
                                unsigned int * vertex_buf_offset_in_bytes, unsigned int * index_buf_offset_in_bytes);

    GeometryManager();
    ~GeometryManager();

    static GeometryManager* Instance();
    static void DestroyInstance();

protected:

    size_t _max_buffer_size;
    bool _new_buffer_per_data;

    class FormatBuffers {
    public:
        //if one of v_format or i_format not null, so buffers is vertex or index
        IVertexFormat* v_format = 0;
        IIndexFormat* i_format = 0;
        unsigned int _usage = 0;
        unsigned int _accessFlag = 0;
        std::vector<GPUBuffer*> gl_buffers;
    };

    std::vector<FormatBuffers*> _buffers;

    //if vfo not null, request vertexbuffer
    //if ifo not null, request indexbuffer
    GPUBuffer* _RequestBuffer(void* data, IVertexFormat* vfo, IIndexFormat* ifo, const size_t& data_size, const unsigned int& usage, const unsigned int& accessFlag, bool * savedToBuffer);
};

}

#endif // _MR_GEOMETRY_BUFFER_H_
