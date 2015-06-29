#pragma once

#ifndef _MR_GEOMETRY_DRAW_PARAMS_H_
#define _MR_GEOMETRY_DRAW_PARAMS_H_

#include "GeometryInterfaces.hpp"

namespace mr {

class GeometryDrawParamsArrays : public IGeometryDrawParams {
public:
    inline void SetVertexStart(const unsigned int& v) override { _cmd.first = v; }
    inline void SetVertexCount(const unsigned int& i) override { _cmd.count = i; }
    inline unsigned int GetVertexStart() const override { return _cmd.first; }
    inline unsigned int GetVertexCount() const override { return _cmd.count; }
    inline void SetInstancesNum(unsigned int const& num) override { _cmd.instanceCount; }
    inline unsigned int GetInstancesNum() const override { return _cmd.instanceCount; }
    inline void SetFirstInstance(unsigned int const& index) override { _cmd.baseInstance = index; }
    inline unsigned int GetFirstInstance() const override { return _cmd.baseInstance; }

    inline void SetIndexStart(const unsigned int& i) override {}
    inline void SetIndexCount(const unsigned int& i) override {}
    inline unsigned int GetIndexStart() const override {return 0;}
    inline unsigned int GetIndexCount() const override {return 0;}

    //inline void* GetIndirectPtr() const override { return 0; }
    inline const void* GetPtr() const override { return &_cmd; }

    inline bool GetUsingIndexBuffer() const override { return false; }

    GeometryDrawParamsArrays(unsigned int const& vStart, unsigned int const& vCount, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);
    virtual ~GeometryDrawParamsArrays();

    static IGeometryDrawParamsPtr Create(unsigned int const& vStart, unsigned int const& vCount, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);
protected:
    DrawArraysIndirectCmd _cmd;
};

class GeometryDrawParamsElements : public IGeometryDrawParams {
public:
    inline void SetVertexStart(const unsigned int& v) override { _cmd.baseVertex = v; }
    inline unsigned int GetVertexStart() const override { return _cmd.baseVertex; }

    inline void SetInstancesNum(unsigned int const& num) override { _cmd.primCount; }
    inline unsigned int GetInstancesNum() const override { return _cmd.primCount; }

    inline void SetFirstInstance(unsigned int const& index) override { _cmd.baseInstance = index; }
    inline unsigned int GetFirstInstance() const override { return _cmd.baseInstance; }

    inline void SetIndexStart(const unsigned int& i) override { _cmd.firstIndex = i; }
    inline unsigned int GetIndexStart() const override { return _cmd.firstIndex; }

    inline void SetIndexCount(const unsigned int& i) override { _cmd.count = i; }
    inline unsigned int GetIndexCount() const override { return _cmd.count; }

    inline void SetVertexCount(const unsigned int& i) override {}
    inline unsigned int GetVertexCount() const override { return 0; }

    //inline void* GetIndirectPtr() const override { return 0; }
    inline const void* GetPtr() const override { return &_cmd; }

    inline bool GetUsingIndexBuffer() const override { return true; }

    GeometryDrawParamsElements(unsigned int const& iStart, unsigned int const& iCount, unsigned int const& vStart, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);
    virtual ~GeometryDrawParamsElements();

    static IGeometryDrawParamsPtr Create(unsigned int const& iStart, unsigned int const& iCount, unsigned int const& vStart, unsigned int const& instancesNum = 1, unsigned int const& firstInstance = 0);
protected:
    DrawElementsIndirectCmd _cmd;
};

}

#endif
