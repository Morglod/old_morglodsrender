#pragma once

#ifndef _MR_GEOMETRY_DATA_H_
#define _MR_GEOMETRY_DATA_H_

#include "GeometryInterfaces.hpp"
#include <memory>

namespace MR {

class VertexData : public IVertexData {
public:
    unsigned int GetDrawMode() override { return _draw_mode; }
    size_t GetSize() override { return _size; }
    void* GetData() override { return _memory; }
    IVertexFormat* GetFormat() override { return _format; }

    VertexData(const unsigned int& drawMode, const size_t& size, void* data, IVertexFormat* format);
    virtual ~VertexData();
protected:
    unsigned int _draw_mode;
    size_t _size;
    void* _memory;
    IVertexFormat* _format;
};

typedef std::shared_ptr<VertexData> VertexDataPtr;

class IndexData : public IIndexData {
public:
    unsigned int GetDrawMode() override { return _draw_mode; }
    size_t GetSize() override { return _size; }
    void* GetData() override { return _memory; }
    IIndexFormat* GetFormat() override { return _format; }

    IndexData(const unsigned int& drawMode, const size_t& size, void* data, IIndexFormat* format);
    virtual ~IndexData();
protected:
    unsigned int _draw_mode;
    size_t _size;
    void* _memory;
    IIndexFormat* _format;
};

typedef std::shared_ptr<IndexData> IndexDataPtr;

}

#endif // _MR_GEOMETRY_DATA_H_
