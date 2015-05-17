#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "../Config.hpp"
#include "GeometryInterfaces.hpp"
#include "GeometryFormats.hpp"

#include <map>

namespace mr {

class GeometryBuffer : public IGeometryBuffer {
public:
    bool SetVertexBuffer(IGPUBuffer* buf) override;
    inline IGPUBuffer* GetVertexBuffer() const override { return _vb; }

    bool SetIndexBuffer(IGPUBuffer* buf) override;
    inline IGPUBuffer* GetIndexBuffer() const override { return _ib; }

    inline IGeometryBuffer::DrawMode GetDrawMode() const override { return _draw_mode; }
    inline void SetDrawMode(const IGeometryBuffer::DrawMode& dm) override { _draw_mode = dm; }

    inline void SetFormat(VertexFormatPtr f, IndexFormatPtr fi) override { _format = f; _iformat = fi; }
    inline VertexFormatPtr GetVertexFormat() const override { return _format; }
    inline IndexFormatPtr GetIndexFormat() const override { return _iformat; }

    void SetAttribute(VertexAttribute const& attrib, IGPUBuffer* buf) override;
    inline IGPUBuffer* GetAttribute(VertexAttribute const& attrib) override { if(_customAttribs.count(attrib) == 0) return nullptr; else return _customAttribs[attrib]; }

    inline bool Good() const override { return _created && (_vb != nullptr) && (_format != nullptr); }

    bool Bind(bool useIndexBuffer) const override;

    bool Create(IGeometryBuffer::CreationParams const& params) override;

    GeometryBuffer();
    virtual ~GeometryBuffer();

    /*
        NVIDIA EXT
    */
    inline bool GetVertexBuffer_NVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) override {
        if(_vb_nv_buffer_size == 0) return false;
        if(nv_resident_ptr) *nv_resident_ptr = _vb_nv_resident_ptr;
        if(nv_buffer_size) *nv_buffer_size = _vb_nv_buffer_size;
        return true;
    }

    inline bool GetIndexBuffer_NVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) override {
        if(_ib_nv_buffer_size == 0) return false;
        if(nv_resident_ptr) *nv_resident_ptr = _ib_nv_resident_ptr;
        if(nv_buffer_size) *nv_buffer_size = _ib_nv_buffer_size;
        return true;
    }
    /*
        #
    */
protected:
    bool _created;

    IGPUBuffer* _vb;
    IGPUBuffer* _ib;
    VertexFormatPtr _format;
    IndexFormatPtr _iformat;
    IGeometryBuffer::DrawMode _draw_mode;

    uint64_t _vb_nv_resident_ptr, _ib_nv_resident_ptr;
    int _vb_nv_buffer_size, _ib_nv_buffer_size;

    std::map<VertexAttribute, IGPUBuffer*> _customAttribs;

    struct BufferResidentPtr {
        uint64_t ptr;
        int size;
    };
    std::map<VertexAttribute, BufferResidentPtr> _customAttribNVPtr;
};

}

#endif
