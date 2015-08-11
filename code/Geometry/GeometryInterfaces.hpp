/**

IGeometryBuffer is interface providing methods working with 'geometry source'.

**/

#pragma once

#ifndef _MR_GEOMETRY_INTERFACES_
#define _MR_GEOMETRY_INTERFACES_

#include "DrawModes.hpp"

#include <memory>

namespace mr {

struct GeomDataType;
struct VertexAttributeDesc;
struct VertexAttribute;
struct VertexFormat;
struct IndexFormat;

typedef std::shared_ptr<GeomDataType> GeomDataTypePtr;
typedef std::shared_ptr<VertexAttributeDesc> VertexAttributeDescPtr;
typedef std::shared_ptr<VertexAttribute> VertexAttributePtr;
typedef std::shared_ptr<VertexFormat> VertexFormatPtr;
typedef std::shared_ptr<IndexFormat> IndexFormatPtr;

class IGeometryBuffer;
typedef std::shared_ptr<IGeometryBuffer> IGeometryBufferPtr;

class VertexBuffer;
typedef std::shared_ptr<VertexBuffer> VertexBufferPtr;

class IndexBuffer;
typedef std::shared_ptr<IndexBuffer> IndexBufferPtr;

class IBuffer;

class IGeometryBuffer {
public:
    struct CreationParams {
    public:
        VertexBufferPtr vertexBuffer = nullptr;
        VertexFormatPtr vertexFormat = nullptr;
        IndexBufferPtr indexBuffer = nullptr;
        IndexFormatPtr indexFormat = nullptr;
        DrawMode drawMode = DrawMode::Triangles;

        CreationParams() {}
        CreationParams(VertexBufferPtr const& vb, IndexBufferPtr const& ib, VertexFormatPtr const& fv, IndexFormatPtr const& fi, DrawMode const& dm) :
            vertexBuffer(vb), vertexFormat(fv), indexBuffer(ib), indexFormat(fi), drawMode(dm) {}
    };

    virtual bool Create(CreationParams const& params) = 0;

    virtual VertexBufferPtr GetVertexBuffer() const = 0;
    virtual IndexBufferPtr GetIndexBuffer() const = 0;
    virtual DrawMode GetDrawMode() const = 0;

    virtual bool Bind(bool useIndexBuffer) const = 0;

    virtual void SetAttribute(VertexAttribute const& attrib, IBuffer* buf) = 0;
    virtual IBuffer* GetAttribute(VertexAttribute const& attrib) = 0;
};

class IGeometryDrawParams {
public:
    typedef struct {
        unsigned int count;
        unsigned int instanceCount;
        unsigned int first;
        unsigned int baseInstance;
    } DrawArraysIndirectCmd;

    typedef struct {
        unsigned int count;
        unsigned int primCount;
        unsigned int firstIndex;
        unsigned int baseVertex;
        unsigned int baseInstance;
    } DrawElementsIndirectCmd;

    virtual void SetIndexStart(const unsigned int& i) = 0;
    virtual void SetVertexStart(const unsigned int& v) = 0;
    virtual void SetIndexCount(const unsigned int& i) = 0;
    virtual void SetVertexCount(const unsigned int& i) = 0;

    virtual unsigned int GetIndexStart() const = 0;
    virtual unsigned int GetVertexStart() const = 0;
    virtual unsigned int GetIndexCount() const = 0;
    virtual unsigned int GetVertexCount() const = 0;

    //By default should be 1
    virtual void SetInstancesNum(unsigned int const& num) = 0;
    virtual unsigned int GetInstancesNum() const = 0;

    //By default should be 0
    virtual void SetFirstInstance(unsigned int const& index) = 0;
    virtual unsigned int GetFirstInstance() const = 0;

    //Pointers may be '0' if not supported or no any.
    //virtual void* GetIndirectPtr() const = 0;
    virtual const void* GetPtr() const = 0; //get pointer to struct (DrawArraysIndirectCmd or DrawElementsIndirectCmd).

    virtual bool GetUsingIndexBuffer() const = 0;

    virtual ~IGeometryDrawParams() {}
};

typedef std::shared_ptr<IGeometryDrawParams> IGeometryDrawParamsPtr;

class IGeometry {
public:
    virtual IGeometryBufferPtr GetGeometryBuffer() const = 0;
    virtual void SetGeometryBuffer(IGeometryBufferPtr const& buffer) = 0;

    virtual IGeometryDrawParamsPtr GetDrawParams() const = 0;
    virtual void SetDrawParams(IGeometryDrawParamsPtr const&params) = 0;

    virtual void Draw() const = 0;
    virtual ~IGeometry() {}
};

}

#endif
