#pragma once

#ifndef _MR_GEOMETRY_MANAGER_H_
#define _MR_GEOMETRY_MANAGER_H_

#include "GeometryInterfaces.hpp"
#include "Singleton.hpp"
#include "../Buffers/VirtualBuffer.hpp"

namespace mr {

class GeometryManager : public mu::Singleton<GeometryManager> {
public:
    IGeometry* PlaceGeometry(VertexFormatPtr const& vertexFormat, void* vertexData, const size_t& vertexNum,
                             IndexFormatPtr const& indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const IGeometryBuffer::DrawMode& drawMode);

    inline bool GetBufferPerGeometry() { return _buffer_per_geom; }
    inline void SetBufferPerGeometry(bool const& b) { _buffer_per_geom = b; }

    GeometryManager();
    ~GeometryManager();

    inline bool _GetNewBufferPerGeometry() { return _buffer_per_geom; }
    inline void _SetNewBufferPerGeometry(bool b) { _buffer_per_geom = b; }

    inline size_t _GetMaxBufferSize() { return _max_buffer_size; }
    inline void _SetMaxBufferSize(size_t const& s) { _max_buffer_size = s; }

protected:
    size_t _max_buffer_size;
    bool _buffer_per_geom;

    struct VertexFormatBuffer {
        VertexFormatPtr format;
        mr::IGPUBuffer::Usage usage;
        mr::VirtualGPUBufferManager* manager; //when memory is totally used, manager will be deleted.
        mr::IGPUBuffer* buffer; //ptr to real buffer
    };

    struct IndexFormatBuffer {
        IndexFormatPtr format;
        mr::IGPUBuffer::Usage usage;
        mr::VirtualGPUBufferManager* manager; //when memory is totally used, manager will be deleted.
        mr::IGPUBuffer* buffer; //ptr to real buffer
    };

    std::vector<VertexFormatBuffer> _vertex_buffers;
    std::vector<IndexFormatBuffer> _index_buffers;

    VertexFormatBuffer* _RequestVFBuffer(VertexFormatPtr const& format, size_t const& dataSize, IGPUBuffer::Usage const& usage);
    IndexFormatBuffer* _RequestIFBuffer(IndexFormatPtr const& format, size_t const& dataSize, IGPUBuffer::Usage const& usage);
};

}

#endif
