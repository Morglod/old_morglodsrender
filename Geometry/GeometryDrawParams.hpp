#pragma once

#ifndef _MR_GEOMETRY_DRAW_PARAMS_H_
#define _MR_GEOMETRY_DRAW_PARAMS_H_

#include "GeometryInterfaces.hpp"

namespace mr {

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

    static IGeometryDrawParamsPtr DrawArrays(const unsigned int& vstart, const unsigned int& vcount);
    static IGeometryDrawParamsPtr DrawElements(const unsigned int& vstart, const unsigned int& istart, const unsigned int& icount);

    GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount);
    virtual ~GeometryDrawParams();
protected:
    void _MakeDrawCmd();

    void* drawCmd;
    unsigned int _istart, _vstart, _icount, _vcount;
    bool _index_buffer;
};

}

#endif
