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

#include "../Buffers/VirtualBuffer.hpp"

namespace MR {

class IRenderSystem;
class Geometry;
class GeometryBuffer;

class GPUGeometryBuffer : public IGPUGeometryBuffer {
public:
    void SetGPUBuffer(IGPUBuffer* buf) override;
    inline IGPUBuffer* GetGPUBuffer() override { return _buffer; }
    void GetNVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) override;

    GPUGeometryBuffer(IGPUBuffer* buf);
    virtual ~GPUGeometryBuffer();
protected:
    IGPUBuffer* _buffer;

    uint64_t _nv_resident_ptr;
    int _nv_buffer_size;
};

class GeometryBuffer : public IGeometryBuffer {
public:
    bool SetVertexBuffer(IGPUGeometryBufferPtr buf) override;
    inline IGPUGeometryBuffer* GetVertexBuffer() override { return _vb.get(); }

    bool SetIndexBuffer(IGPUGeometryBufferPtr buf) override;
    inline IGPUGeometryBuffer* GetIndexBuffer() override { return _ib.get(); }

    unsigned int GetDrawMode() override { return _draw_mode; }
    void SetDrawMode(const unsigned int& dm) override { _draw_mode = dm; }

    inline void SetFormat(IVertexFormat* f, IIndexFormat* fi) override { _format = f; _iformat = fi; }
    inline IVertexFormat* GetVertexFormat() override { return _format; }
    inline IIndexFormat* GetIndexFormat() override { return _iformat; }

    void Release() override;

    unsigned int GetVAO() override { return _vao; }

    GeometryBuffer(IGPUGeometryBufferPtr vb, IGPUGeometryBufferPtr ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode);
    virtual ~GeometryBuffer();
protected:
    IGPUGeometryBufferPtr _vb;
    IGPUGeometryBufferPtr _ib;
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
    static IGeometryDrawParamsPtr DrawElements(const unsigned int& istart, const unsigned int& icount, const unsigned int& vstart);

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

    static IGeometry* MakeTriangle(const float& scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
    static IGeometry* MakeQuad(const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& texCoords = true, const glm::vec2& texCoordsScale = glm::vec2(1.0f, 1.0f));
    static IGeometry* MakeBox(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f), const bool& inside = false);
protected:
    IGeometryBuffer* _buffer;
    IGeometryDrawParamsPtr _draw_params;
};

class GeometryManager : public Singleton<GeometryManager> {
public:
    IGeometry* PlaceGeometry(IVertexFormat* vertexFormat, void* vertexData, const size_t& vertexNum,
                             IIndexFormat* indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const unsigned int& drawMode);

    inline bool GetBufferPerGeometry() { return _buffer_per_geom; }
    inline void SetBufferPerGeometry(bool const& b) { _buffer_per_geom = b; }

    GeometryManager();
    ~GeometryManager();
protected:
    size_t _max_buffer_size;
    bool _buffer_per_geom;
    bool _map_global_buffers;

    class FormatBuffer {
    public:
        IVertexFormat* vFormat;
        IIndexFormat* iFormat;
        MR::IGPUBuffer::Usage usage;
        MR::VirtualGPUBufferManager* manager; //when memory is totally used, manager will be deleted.
        MR::IGPUBuffer* buffer; //ptr to real buffer
    };

    std::vector<FormatBuffer> _buffers;

    FormatBuffer* _RequestFormatBuffer(IVertexFormat* vertexFormat, const size_t& vertexDataSize,
                             IIndexFormat* indexFormat, const size_t& indexDataSize,
                             const IGPUBuffer::Usage& usage);
};

}

#endif // _MR_GEOMETRY_BUFFER_H_
