#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "../Types.hpp"
#include "../Config.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

#include <vector>

#include "GeometryFormats.hpp"
#include "GeometryInterfaces.hpp"

namespace MR {

class IRenderSystem;
class Geometry;
class GeometryBuffer;

class GPUBuffer : public IBuffer {
public:
    void BindToStream(IGeometryStream* stream, const unsigned int& offset) override;

    bool Buffer(void* data, const unsigned int& size, const unsigned int& usage) override;
    bool BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) override;
    bool Update(void* data, const unsigned int& size, const unsigned int& offset) override;
    bool GetBuffered(void* data, const unsigned int& offset, const unsigned int& size) override;
    void Release() override;

    inline size_t GetAllocatedSize() override { return _allocated_size; }
    inline size_t GetUsedSize() override { return _used_size; }
    inline size_t GetFreeSize() override { return _free_size; }
    inline unsigned int GetNextFreeOffset() override { return _next_free_offset; }

    bool _CopyTo(const unsigned int& dstHandle, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) override;

    inline void SetNum(const unsigned int & num) { _num = num; }
    inline unsigned int GetNum() { return _num; }

    unsigned int GetGPUHandle() override { return _handle; }
    inline uint64_t _GetResidentPtr() { return _resident_ptr; }
    inline int _GetBufferSize() { return _buffer_size; }

    GPUBuffer(const unsigned int& target);
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

class GeometryBuffer : public IGeometryBuffer {
public:
    bool SetVertexBuffer(GPUBuffer* buf) override;
    inline GPUBuffer* GetVertexBuffer() override { return _vb; }

    bool SetIndexBuffer(GPUBuffer* buf) override;
    inline GPUBuffer* GetIndexBuffer() override { return _ib; }

    unsigned int GetDrawMode() override { return _draw_mode; }
    void SetDrawMode(const unsigned int& dm) override { _draw_mode = dm; }

    inline void SetFormat(IVertexFormat* f, IIndexFormat* fi) override { _format = f; _iformat = fi; }
    inline IVertexFormat* GetVertexFormat() override { return _format; }
    inline IIndexFormat* GetIndexFormat() override { return _iformat; }

    void Release() override;

    unsigned int GetVAO() override { return _vao; }

    GeometryBuffer(GPUBuffer* vb, GPUBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode);
    virtual ~GeometryBuffer();
protected:
    GPUBuffer* _vb;
    GPUBuffer* _ib;
    IVertexFormat* _format;
    IIndexFormat* _iformat;
    unsigned int _vao;
    unsigned int _draw_mode;
};

class GeometryDrawParams : public IGeometryDrawParams {
public:
    void SetIndexStart(const unsigned int& i) override;
    void SetVertexStart(const unsigned int& v) override;
    void SetIndexCount(const unsigned int& i) override;
    void SetVertexCount(const unsigned int& i) override;

    unsigned int GetIndexStart() override { return _istart; }
    unsigned int GetVertexStart() override { return _vstart; }
    unsigned int GetIndexCount() override { return _icount; }
    unsigned int GetVertexCount() override { return _vcount; }

    void* GetIndirectPtr() override { return drawCmd; }

    void SetUseIndexBuffer(const bool& state) override;
    bool GetUseIndexBuffer() override;

    void BeginDraw() override;
    void EndDraw() override;

    static IGeometryDrawParamsPtr DrawArrays(const unsigned int& vstart, const unsigned int& vcount);
    static IGeometryDrawParamsPtr DrawElements(const unsigned int& istart, const unsigned int& icount);

    GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount);
    virtual ~GeometryDrawParams();
protected:
    void _MakeDrawCmd();

    void* drawCmd;
    unsigned int _istart, _vstart, _icount, _vcount;
    bool _index_buffer;

    unsigned int _indirect_buffer;
};

class Geometry : public IGeometry {
public:
    IGeometryBuffer* GetGeometryBuffer() override { return _buffer; }
    void SetGeometryBuffer(IGeometryBuffer* buffer) override;

    IGeometryDrawParamsPtr GetDrawParams() override { return _draw_params; }
    void SetDrawParams(IGeometryDrawParamsPtr params) override { _draw_params = params; }

    void Draw() override;

    Geometry(IGeometryBuffer* buffer, IGeometryDrawParamsPtr params);
    virtual ~Geometry();

    static Geometry* MakeTriangle(const float& scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
    static Geometry* MakeQuad(const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& texCoords = true, const glm::vec2& texCoordsScale = glm::vec2(1.0f, 1.0f));
    static Geometry* MakeBox(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& inside = false);
protected:
    IGeometryBuffer* _buffer;
    IGeometryDrawParamsPtr _draw_params;
};

class GeometryManager : public Singleton<GeometryManager> {
public:
    GeometryBuffer * PlaceGeometry(  IVertexFormat* vfo, IIndexFormat* ifo,
                                void* vert_data, const size_t& vert_data_size,
                                void* ind_data, const size_t& ind_data_size,
                                const unsigned int& usage, const unsigned int& accessFlag, const unsigned int& drawMode,
                                unsigned int * vertex_buf_offset_in_bytes, unsigned int * index_buf_offset_in_bytes);

    GeometryManager();
    ~GeometryManager();
protected:

    size_t _max_buffer_size;
    bool _new_buffer_per_data;

    class FormatBuffers {
    public:
        IVertexFormat* v_format = 0;
        unsigned int _usage = 0;
        unsigned int _accessFlag = 0;
        std::vector<GPUBuffer*> gl_buffers;
    };

    std::vector<FormatBuffers*> _buffers;

    //if vfo not null, request vertexbuffer
    //if ifo not null, request indexbuffer
    GPUBuffer* _RequestVBuffer(void* data, IVertexFormat* vfo, const size_t& data_size, const unsigned int& usage, const unsigned int& accessFlag, bool * savedToBuffer);
};

}

#endif // _MR_GEOMETRY_BUFFER_H_
