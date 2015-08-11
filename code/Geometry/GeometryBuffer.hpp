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
    inline VertexBufferPtr GetVertexBuffer() const override { return _vb; }
    inline IndexBufferPtr GetIndexBuffer() const override { return _ib; }
    inline DrawMode GetDrawMode() const override { return _drawMode; }

    void SetAttribute(VertexAttribute const& attrib, IBuffer* buf) override;
    inline IBuffer* GetAttribute(VertexAttribute const& attrib) override { if(_customAttribs.count(attrib) == 0) return nullptr; else return _customAttribs[attrib].buffer; }

    bool Bind(bool useIndexBuffer) const override;

    bool Create(IGeometryBuffer::CreationParams const& params) override;

    GeometryBuffer();
    virtual ~GeometryBuffer();

protected:

    bool _SetVertexBuffer(VertexBufferPtr const& vb);
    bool _SetIndexBuffer(IndexBufferPtr const& ib);

    VertexBufferPtr _vb;
    IndexBufferPtr _ib;
    DrawMode _drawMode;

    struct CustomAttribute {
        IBuffer* buffer = nullptr;
        uint64_t ptr = 0;
        int size = 0;
    };

    std::map<VertexAttribute, CustomAttribute> _customAttribs;
};

}

#endif
