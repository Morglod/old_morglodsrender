#include "GeometryBufferV2.hpp"
#include "MachineInfo.hpp"
#include "RenderSystem.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

IVertexDataType* VertexDataTypeInt::Instance(){
    static IVertexDataType* inst = new VertexDataTypeInt;
    return inst;
}

IVertexDataType* VertexDataTypeUInt::Instance(){
    static IVertexDataType* inst = new VertexDataTypeUInt;
    return inst;
}

IVertexDataType* VertexDataTypeFloat::Instance(){
    static IVertexDataType* inst = new VertexDataTypeFloat;
    return inst;
}

VertexDataTypeCustom::VertexDataTypeCustom(const unsigned int& gpu_type, const unsigned char& size) :
    _gpu_type(gpu_type), _size(size) {}

VertexAttributeCustom::VertexAttributeCustom(const unsigned char& elementsNum, IVertexDataType* dataType, const unsigned int& shaderIndex) :
    _el_num(elementsNum), _data_type(dataType), _size( ((uint64_t)_el_num) * ((uint64_t)_data_type->Size()) ), _shaderIndex(shaderIndex) {}

void VertexFormatCustom::AddVertexAttribute(IVertexAttribute* a) {
    if(_pointers.size() == 0) _pointers.push_back(0);
    else _pointers.push_back(_nextPtr);
    _attribs.push_back(a);
    _size += (uint64_t)a->Size();
    _nextPtr = (uint64_t)_size;
}

bool VertexFormatCustom::Bind() {
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glVertexAttribFormatNV(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size());
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
        }

        glEnableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    }
    else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glVertexAttribPointer(_attribs[i]->ShaderIndex(), (int)_attribs[i]->ElementsNum(), _attribs[i]->DataType()->GPUDataType(), GL_FALSE, this->Size(), (void*)_pointers[i]);
            glEnableVertexAttribArray(_attribs[i]->ShaderIndex());
        }
    }

    return true;
}

void VertexFormatCustom::UnBind() {
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }

        glDisableClientState(GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV);
    } else {
        for(size_t i = 0; i < _attribs.size(); ++i) {
            glDisableVertexAttribArray(_attribs[i]->ShaderIndex());
        }
    }
}

bool VertexFormatCustom::Equal(IVertexFormat* vf){
    if(Size() != vf->Size()) return false;
    std::vector<IVertexAttribute*>* attrArray = _Attributes();
    size_t i2 = 0;
    for(size_t i = 0; i < _attribs.size(); ++i){
        if( !(_attribs[i]->Equal(attrArray[0][i2])) ) return false;
        ++i2;
    }
    return true;
}

VertexFormatCustom::VertexFormatCustom() : _attribs(), _pointers(), _nextPtr(0), _size(0) {
}

VertexFormatCustom::~VertexFormatCustom(){
}

bool IndexFormatCustom::Bind(){
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        glEnableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
    return true;
}

void IndexFormatCustom::UnBind(){
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        glDisableClientState(GL_ELEMENT_ARRAY_UNIFIED_NV);
    }
}

IndexFormatCustom::IndexFormatCustom(IVertexDataType* dataType) : _dataType(dataType) {
}

IndexFormatCustom::~IndexFormatCustom(){
}

bool VertexBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag){
    _usage = usage;
    _accessFlag = accessFlag;
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
    }
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, data, usage);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _handle);
        glBufferData(GL_ARRAY_BUFFER, size, data, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(_handle, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetNamedBufferParameterivEXT(_handle, GL_BUFFER_SIZE, &_buffer_size);
            //glMakeNamedBufferResidentNV(_handle, accessFlag);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, _handle);
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_buffer_size);
            //glMakeBufferResidentNV(GL_ARRAY_BUFFER, accessFlag);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
    return true;
}

void VertexBuffer::Release(){
    if(_handle != 0){
        glDeleteBuffers(1, &_handle);
        _handle = 0;
    }
}

VertexBuffer::VertexBuffer(GL::IContext* ctx) : Object(), IGLObject(ctx, GL::IGLObject::ObjectType::Buffer, &_handle), _usage(0), _accessFlag(0), _handle(0), _buffer_size(0), _num(0) {
}

VertexBuffer::~VertexBuffer(){
    Release();
}

bool IndexBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag){
    _usage = usage;
    _accessFlag = accessFlag;
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
    }
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, data, usage);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, usage);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(_handle, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetNamedBufferParameterivEXT(_handle, GL_BUFFER_SIZE, &_buffer_size);
            //glMakeNamedBufferResidentNV(_handle, accessFlag);
        } else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _handle);
            glGetBufferParameterui64vNV(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &_buffer_size);
            //glMakeBufferResidentNV(GL_ELEMENT_ARRAY_BUFFER, accessFlag);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    return true;
}

void IndexBuffer::Release(){
    if(_handle != 0){
        glDeleteBuffers(1, &_handle);
        _handle = 0;
    }
}

IndexBuffer::IndexBuffer(GL::IContext* ctx) : Object(), IGLObject(ctx, GL::IGLObject::ObjectType::Buffer, &_handle), _usage(0), _accessFlag(0), _handle(0), _buffer_size(0), _num(0) {
}

IndexBuffer::~IndexBuffer(){
    Release();
}

GeometryBuffer* GeometryBuffer::Plane(GL::IContext* ctx, const glm::vec3& scale, const glm::vec3 pos, const unsigned int& usage, const unsigned int& drawm){
    const int per_verts_elements = 5;
    const int garray_size = per_verts_elements * 4;
    float garray[garray_size] {
        //v1
        -0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y, -0.5f*scale.y + pos.z, //pos
        0.0f, 0.0f, //tex coord
        //v2
        -0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y,  0.5f*scale.y + pos.z,
        0.0f, 1.0f,
        //v4
        0.5f*scale.x + pos.x,  0.5f*scale.y + pos.y,  0.5f*scale.y + pos.z,
        1.0f, 1.0f,
        //v3
        0.5f*scale.x + pos.x, -0.5f*scale.y + pos.y, -0.5f*scale.y + pos.z,
        1.0f, 0.0f
    };

    //Create index array for 2 triangles

    int iarray_size = 4;
    unsigned int* iarray = 0;

    switch(drawm){
    case IGeometryBuffer::Draw_Points:
    case IGeometryBuffer::Draw_LineLoop:
    case IGeometryBuffer::Draw_LineStrip:
    case IGeometryBuffer::Draw_Quads:
        iarray = new unsigned int[4] { 0, 1, 2, 3 };
        iarray_size = 4;
        break;
    case IGeometryBuffer::Draw_Triangles:
        iarray = new unsigned int[6] { 0, 1, 2, 2, 3, 0 };
        iarray_size = 6;
        break;
    }

    //Create vertex declaration that tells engine in wich sequence data is stored
    MR::VertexFormatCustom* vformat = new VertexFormatCustom();
    vformat->AddVertexAttribute(new MR::VertexAttributeCustom(3, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_Position));
    vformat->AddVertexAttribute(new MR::VertexAttributeCustom(2, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_TexCoord));

    MR::VertexBuffer* vbuffer = new VertexBuffer(ctx);
    vbuffer->Buffer(&garray[0], sizeof(float)*garray_size, usage, drawm);
    vbuffer->SetNum(4); //set vertexes num

    MR::IndexFormatCustom* iformat = new IndexFormatCustom(VertexDataTypeUInt::Instance());

    MR::IndexBuffer* ibuffer = new IndexBuffer(ctx);
    ibuffer->Buffer(&iarray[0], sizeof(unsigned int)*iarray_size, usage, drawm);
    ibuffer->SetNum(iarray_size);

    //Create geometry buffer
    MR::GeometryBuffer* gb = new GeometryBuffer(ctx, vbuffer, ibuffer, vformat, iformat, drawm); //new MR::GeometryBuffer(vDecl, iDecl, &garray[0], sizeof(float)*garray_size, &iarray[0], sizeof(unsigned int)*iarray_size, garray_size / per_verts_elements, iarray_size, usage, usage, drawm);
    delete [] iarray;

    return gb;
}

bool GeometryBuffer::SetVertexBuffer(VertexBuffer* buf) {
    glBindVertexArray(_vao);
    _vb = buf;
    if(buf == nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, buf->_handle);
        if(!_format->Bind()) return false;
    }
    glBindVertexArray(0);
    return true;
}

bool GeometryBuffer::SetIndexBuffer(IndexBuffer* buf) {
    glBindVertexArray(_vao);
    _ib = buf;
    if(buf == nullptr) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    } else {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->_handle);
        if(!_iformat->Bind()) return false;
    }
    glBindVertexArray(0);
    return true;
}

void GeometryBuffer::Draw(IRenderSystem* rc, const unsigned int& start, const unsigned int& end, const int& count) {
    if(_format == nullptr) return;
    if(_vb == nullptr) return;

    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        rc->BindVertexFormat(_format);

        for(size_t i = 0; i < _format->_Attributes()->size(); ++i) {
            glBufferAddressRangeNV(GL_VERTEX_ATTRIB_ARRAY_ADDRESS_NV,
                                   (*(_format->_Attributes()))[i]->ShaderIndex(),
                                   _vb->_resident_ptr + (*(_format->_Offsets()))[i],
                                   _vb->_buffer_size
                                   );
        }

        if(_ib != nullptr){
            rc->BindIndexFormat(_iformat);
            glBufferAddressRangeNV(GL_ELEMENT_ARRAY_ADDRESS_NV, 0, _ib->_resident_ptr, _ib->_buffer_size);

            /*if(MR::MachineInfo::Feature_DrawIndirect()) {
                typedef struct {
                    unsigned int count;
                    unsigned int primCount;
                    unsigned int firstIndex;
                    unsigned int baseVertex;
                    unsigned int baseInstance;
                } SDrawIndirectCommand;
                SDrawIndirectCommand cmd {
                    count, 1, start, 0, 0
                };
                glDrawElementsIndirect(_draw_mode, _iformat->GetDataType()->GPUDataType(), &cmd);
            } else {*/
                glDrawRangeElements(_draw_mode,
                                    start,
                                    end,
                                    count,
                                    _iformat->GetDataType()->GPUDataType(),
                                    0);
            //}
        }
        else {
            /*if(MR::MachineInfo::Feature_DrawIndirect()) {
                typedef  struct {
                    //Arrays
                    unsigned int acount;
                    unsigned int ainstanceCount;
                    unsigned int afirst;
                    unsigned int abaseInstance;
                } SDrawIndirectCommand;
                SDrawIndirectCommand cmd {
                    count, 0, start, 0
                };
                glDrawArraysIndirect(_draw_mode, &cmd);
            } else {*/
                glDrawArrays(_draw_mode, 0, _vb->_num);
            //}
        }
    }
    else {
        glBindVertexArray(_vao);
        if(_ib != nullptr) {
            /*if(MR::MachineInfo::Feature_DrawIndirect()) {
                glDrawElementsIndirect(_draw_mode, _iformat->GetDataType()->GPUDataType(), (void*)16);
            } else {*/
                glDrawRangeElements(_draw_mode,
                                    start,
                                    end,
                                    count,
                                    _iformat->GetDataType()->GPUDataType(),
                                    0);
            //}
        }
        else {
            /*if(MR::MachineInfo::Feature_DrawIndirect()) {
                glDrawArraysIndirect(_draw_mode, 0);
            } else {*/
                glDrawArrays(_draw_mode, 0, _vb->_num);
            //}
        }

        glBindVertexArray(0);
    }
}

void GeometryBuffer::Release() {
    if(_vao != 0) glDeleteVertexArrays(1, &_vao);
    _vao = 0;
}

GeometryBuffer::GeometryBuffer(GL::IContext* ctx, VertexBuffer* vb, IndexBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode) :
    IGLObject(ctx, GL::IGLObject::ObjectType::Buffer, &_vao), _vb(vb), _ib(ib), _format(f), _iformat(fi), _vao(0), _draw_mode(drawMode) {

    if(!MR::MachineInfo::FeatureNV_GPUPTR()){
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        if(vb == nullptr) glBindBuffer(GL_ARRAY_BUFFER, 0);
        else {
            glBindBuffer(GL_ARRAY_BUFFER, vb->_handle);
            if(f != nullptr) f->Bind();
        }

        if(ib != nullptr){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->_handle);
            if(fi != nullptr) fi->Bind();
        }

        glBindVertexArray(0);
    }
}

GeometryBuffer::~GeometryBuffer(){
    Release();
}

void Geometry::SetGeometryBuffer(IGeometryBuffer* buffer) {
    _buffer = buffer;
}

void Geometry::SetStart(const unsigned int& i) {
    _start = i;
}

void Geometry::SetEnd(const unsigned int& i) {
    _end = i;
}

void Geometry::SetCount(const int& i) {
    _count = i;
}

void Geometry::Draw(IRenderSystem* rc){
    if(!_buffer) return;
    /*if(MR::MachineInfo::Feature_DrawIndirect()) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _draw_buffer->_handle);
    }*/
    _buffer->Draw(rc, _start, _end, _count);
    /*if(MR::MachineInfo::Feature_DrawIndirect()) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }*/
}

Geometry::Geometry(IGeometryBuffer* buffer, const unsigned int& istart, const unsigned int& iend, const int& icount)
 : _buffer(buffer), _start(istart), _end(iend), _count(icount) {
     //if(MR::MachineInfo::Feature_DrawIndirect()) _MakeBuffer();
}

Geometry::~Geometry(){
}

}
