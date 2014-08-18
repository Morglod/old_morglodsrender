#include "InstancedGeometry.hpp"
#include "../MachineInfo.hpp"
#include "GeometryBufferV2.hpp"

#include <GL/glew.h>

namespace MR {
/*
void InstancedDataFormatFixed::SetAttributesNum(const size_t& num) {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;

    _attribs = new IVertexAttribute*[num];
    _pointers = new uint64_t[num];
    _size = 0;
    _nextIndex = 0;
    _attribsNum = num;
}

bool InstancedDataFormatFixed::Equal(IInstancedDataFormat* vf) {
    if(_size != vf->Size()) return false;

    StaticArray<IVertexAttribute*> attrArray = vf->_Attributes();

    size_t i2 = 0;
    for(size_t i = 0; i < attrArray.GetNum(); ++i){
        if( !(_attribs[i]->Equal(attrArray.At(i2))) ) return false;
        ++i2;
    }
    return true;
}

bool InstancedDataFormatFixed::Bind() {
    for(size_t i = 0; i < _attribsNum; ++i) {
        glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
        glVertexAttribPointer(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size(), (void*)_pointers[i]);
        glVertexAttribDivisor(_attribs[i]->ShaderIndex(), 1);
    }
    return true;
}

void InstancedDataFormatFixed::UnBind() {
    for(size_t i = 0; i < _attribsNum; ++i) {
        glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
    }
}

InstancedDataFormatFixed::InstancedDataFormatFixed() : _attribs(nullptr), _pointers(nullptr), _size(0), _nextIndex(0), _attribsNum(0)  {
}

InstancedDataFormatFixed::~InstancedDataFormatFixed() {
    if(_attribs) delete [] _attribs;
    if(_pointers) delete [] _pointers;
}

void InstancedDataFormatFixed::_RecalcSize() {
    _size = 0;
    for(size_t i = 0; i < _nextIndex+1; ++i) {
        _pointers[i] = (uint64_t)_size;
        _size += _attribs[i]->Size();
    }
}

void InstancedDataFormatFixed::SetAttribute(IVertexAttribute* a, const size_t& index) {
    if(index == _attribsNum) return;
    _attribs[index] = a;
    this->_RecalcSize();
}

void InstancedGeometry::SetGeometryBuffer(IGeometryBuffer* buffer) {
    _buffer = buffer;
    _MakeDrawCmd();
}

void InstancedGeometry::SetIStart(const unsigned int& i) {
    _MakeDrawCmd();
}

void InstancedGeometry::SetVStart(const unsigned int& v) {
    _MakeDrawCmd();
}

void InstancedGeometry::SetICount(const int& i) {
    _MakeDrawCmd();
}

void InstancedGeometry::SetVCount(const int& i) {
    _MakeDrawCmd();
}

void InstancedGeometry::Draw() {
    glBindBuffer(GL_ARRAY_BUFFER, _instancedBuffer->GetGPUHandle());
    _instanced_format->Bind();
    if(MR::MachineInfo::Feature_DrawIndirect()) _buffer->Draw(drawCmd);
    else _buffer->Draw(_istart, _vstart, _icount, _vcount);
    _instanced_format->UnBind();
}

InstancedGeometry::InstancedGeometry(IGeometryBuffer* buffer, const unsigned int& index_start, const unsigned int& vertex_start, const int& ind_count, const int& vert_count, GPUBuffer* instancedBuffer, const unsigned int& instancesNum, IInstancedDataFormat* instanced_format)
 : _buffer(buffer), _istart(index_start), _vstart(vertex_start), _icount(ind_count), _vcount(vert_count), _instancedBuffer(instancedBuffer), _instancesNum(instancesNum), _instanced_format(instanced_format) {
     _MakeDrawCmd();
}

InstancedGeometry::~InstancedGeometry() {
    if(drawCmd) {
        //if(_buffer->GetIndexBuffer()) {
        //    delete ((DrawElementsIndirectCmd*)drawCmd);
        //    drawCmd = nullptr;
        //} else {
        //    delete ((DrawArraysIndirectCmd*)drawCmd);
        //    drawCmd = nullptr;
        //}
    }
}

void InstancedGeometry::_MakeDrawCmd() {
    if(MR::MachineInfo::Feature_DrawIndirect()) {
        //if(_buffer->GetIndexBuffer()) {
        //    if(drawCmd) {
        //        delete ((DrawElementsIndirectCmd*)drawCmd);
        //    }
        //    drawCmd = new MR::IGeometry::DrawElementsIndirectCmd {_icount, _instancesNum, _istart, _vstart, 0};
        //} else {
        //    if(drawCmd) {
        //        delete ((DrawArraysIndirectCmd*)drawCmd);
        //    }
        //    drawCmd = new MR::IGeometry::DrawArraysIndirectCmd {(unsigned int)_vcount, _instancesNum, _vstart, 0};
        //}
    }
}
*/
}
