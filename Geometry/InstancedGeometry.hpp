#pragma once

#ifndef _MR_INSTANCED_GEOMETRY_H_
#define _MR_INSTANCED_GEOMETRY_H_

#include "GeometryInterfaces.hpp"

namespace MR {

class GPUBuffer;
/*
class InstancedDataFormatFixed : public IInstancedDataFormat {
public:

    /// call this after creating new object of this type
    /// resets all data
    void SetAttributesNum(const size_t& num);

    inline size_t Size() override { return _size; }
    void AddAttribute(IVertexAttribute* a) override {  SetAttribute(a, _nextIndex); ++_nextIndex;  }

    bool Equal(IInstancedDataFormat* vf) override;

    bool Bind() override;
    void UnBind() override;

    StaticArray<IVertexAttribute*> _Attributes() override { return StaticArray<IVertexAttribute*>(&_attribs[0], _attribsNum, false); }
    inline StaticArray<uint64_t> _Offsets() override { return StaticArray<uint64_t>(&_pointers[0], _attribsNum, false); }

    InstancedDataFormatFixed();
    ~InstancedDataFormatFixed();
protected:
    void _RecalcSize();
    void SetAttribute(IVertexAttribute* a, const size_t& index);

    IVertexAttribute** _attribs;
    uint64_t* _pointers;
    size_t _size;
    size_t _nextIndex;
    size_t _attribsNum;
};

class InstancedGeometry : public IInstancedGeometry {
public:
    GPUBuffer* GetInstancedBuffer() override { return _instancedBuffer; }
    void SetInstancedBuffer(GPUBuffer* buf) override { _instancedBuffer = buf; }

    IInstancedDataFormat* GetInstancedDataFormat() override { return _instanced_format; }
    void SetInstancedDataFormat(IInstancedDataFormat* format) override { _instanced_format = format; }

    IGeometryBuffer* GetGeometryBuffer() override { return _buffer; }
    void SetGeometryBuffer(IGeometryBuffer* buffer) override;
    void SetIStart(const unsigned int& i) override;
    void SetVStart(const unsigned int& v) override;
    void SetICount(const int& i) override;
    void SetVCount(const int& i) override;
    void Draw() override;

    InstancedGeometry(IGeometryBuffer* buffer, const unsigned int& index_start, const unsigned int& vertex_start, const int& ind_count, const int& vert_count, GPUBuffer* instancedBuffer, const unsigned int& instancesNum, IInstancedDataFormat* instanced_format);
    virtual ~InstancedGeometry();

    static InstancedGeometry* MakeInstancedTriangle(const float& scale = 1.0f, const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
    static InstancedGeometry* MakeInstancedQuad(const glm::vec2& scale = glm::vec2(1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
    static InstancedGeometry* MakeInstancedBox(const glm::vec3& scale = glm::vec3(1.0f, 1.0f, 1.0f), const glm::vec3& offset = glm::vec3(0.0f, 0.0f, 0.0f));
protected:
    void _MakeDrawCmd();

    IGeometryBuffer* _buffer;
    void* drawCmd;
    unsigned int _istart, _vstart;
    int _icount, _vcount;

    GPUBuffer* _instancedBuffer;
    unsigned int _instancesNum;

    IInstancedDataFormat* _instanced_format;
};
*/
}

#endif // _MR_INSTANCED_GEOMETRY_H_
