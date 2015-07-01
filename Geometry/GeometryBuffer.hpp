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
    inline IGPUBuffer* GetAttribute(VertexAttribute const& attrib) override { if(_customAttribs.count(attrib) == 0) return nullptr; else return _customAttribs[attrib].buffer; }

    bool Bind(bool useIndexBuffer) const override;

    bool Create(IGeometryBuffer::CreationParams const& params) override;

    GeometryBuffer();
    virtual ~GeometryBuffer();

protected:
    IGPUBuffer* _vb;
    IGPUBuffer* _ib;
    VertexFormatPtr _format;
    IndexFormatPtr _iformat;
    IGeometryBuffer::DrawMode _draw_mode;

    struct CustomAttribute {
        IGPUBuffer* buffer = nullptr;
        uint64_t ptr = 0;
        int size = 0;
    };

    std::map<VertexAttribute, CustomAttribute> _customAttribs;
};

}

#endif
