#pragma once

#ifndef _MR_GEOMETRY_MANAGER_H_
#define _MR_GEOMETRY_MANAGER_H_

#include "GeometryInterfaces.hpp"
#include "../Utils/Singleton.hpp"
#include "../Buffers/VirtualBuffer.hpp"

namespace MR {

class GeometryManager : public Singleton<GeometryManager> {
public:
    IGeometry* PlaceGeometry(IVertexFormat* vertexFormat, void* vertexData, const size_t& vertexNum,
                             IIndexFormat* indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const IGeometryBuffer::DrawModes& drawMode);

    inline bool GetBufferPerGeometry() { return _buffer_per_geom; }
    inline void SetBufferPerGeometry(bool const& b) { _buffer_per_geom = b; }

    GeometryManager();
    ~GeometryManager();

    inline bool _GetNewBufferPerGeometry() { return _buffer_per_geom; }
    inline void _SetNewBufferPerGeometry(bool b) { _buffer_per_geom = b; }

    inline size_t _GetMaxBufferSize() { return _max_buffer_size; }
    inline void _SetMaxBufferSize(size_t const& s) { _max_buffer_size = s; }

    inline bool _GetSplitByDataFormats() { return _split_by_data_formats; }
    inline void _SetSplitByDataFormats(bool b) { _split_by_data_formats = b; }
protected:
    size_t _max_buffer_size;
    bool _buffer_per_geom;
    bool _split_by_data_formats;

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

#endif
