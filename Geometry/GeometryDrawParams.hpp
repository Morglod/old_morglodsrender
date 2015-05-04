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

    unsigned int GetIndexStart() const override { return _istart; }
    unsigned int GetVertexStart() const override { return _vstart; }
    unsigned int GetIndexCount() const override { return _icount; }
    unsigned int GetVertexCount() const override { return _vcount; }

    void SetInstancesNum(unsigned int const& num) override;
    inline unsigned int GetInstancesNum() const override { return _instNum; }

    void SetFirstInstance(unsigned int const& index) override;
    unsigned int GetFirstInstance() const override { return _firstInstance; }

    void* GetIndirectPtr() const override { return drawCmd; }

    void SetUsingIndexBuffer(const bool& state) override;
    bool GetUsingIndexBuffer() const override;

    static IGeometryDrawParamsPtr DrawArrays(const unsigned int& vstart, const unsigned int& vcount, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);
    static IGeometryDrawParamsPtr DrawElements(const unsigned int& vstart, const unsigned int& istart, const unsigned int& icount, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);

    GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount, unsigned int const& instancesNum, unsigned int const& firstInstance);
    virtual ~GeometryDrawParams();
protected:
    void _MakeDrawCmd();

    void* drawCmd;
    unsigned int _istart, _vstart, _icount, _vcount, _instNum;
    bool _index_buffer;
    unsigned int _firstInstance;
};

}

#endif
