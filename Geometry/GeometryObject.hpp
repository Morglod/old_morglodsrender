#pragma once

#ifndef _MR_GEOMETRY_OBJECT_H_
#define _MR_GEOMETRY_OBJECT_H_

#include "GeometryInterfaces.hpp"

namespace mr {

class Geometry : public IGeometry {
public:
    IGeometryBufferPtr GetGeometryBuffer() const override { return _buffer; }
    void SetGeometryBuffer(IGeometryBufferPtr const& buffer) override;

    IGeometryDrawParamsPtr GetDrawParams() const override { return _draw_params; }
    void SetDrawParams(IGeometryDrawParamsPtr const& params) override { _draw_params = params; }

    void Draw() const override;

    Geometry(IGeometryBufferPtr const& buffer, IGeometryDrawParamsPtr const& params);
    virtual ~Geometry();

protected:
    IGeometryBufferPtr _buffer;
    IGeometryDrawParamsPtr _draw_params;
};

}

#endif
