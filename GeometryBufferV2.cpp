#include "GeometryBufferV2.hpp"
#include "MachineInfo.hpp"
#include "RenderSystem.hpp"
#include "Utils/Log.hpp"

#include "GL/Context.hpp"

#ifndef __glew_h__
#   define GLEW_STATIC
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

bool IndexFormatCustom::Equal(IIndexFormat* ifo) {
    if(ifo->Size() != this->Size()) return false;
    return (ifo->GetDataType()->Equal(this->GetDataType()));
}

IndexFormatCustom::IndexFormatCustom(IVertexDataType* dataType) : _dataType(dataType) {
}

IndexFormatCustom::~IndexFormatCustom(){
}

bool GPUBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& usage, const unsigned int& accessFlag){
    _usage = usage;
    _accessFlag = accessFlag;
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
        _allocated_size = size;
        _next_free_offset = size;
        _used_size = size;
        _free_size = 0;
    }
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, data, usage);
    } else {
        glBindBuffer(_target, _handle);
        glBufferData(_target, size, data, usage);
        glBindBuffer(_target, 0);
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

bool GPUBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& offset) {
    if(_handle == 0) return false;

    if(offset < _next_free_offset) _used_size += offset - _next_free_offset + size;
    else _used_size += size;

    _free_size -= size;
    _next_free_offset = offset + size;

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferSubDataEXT(_handle, offset, size, data);
    } else {
        glBindBuffer(_target, _handle);
        glBufferSubData(_target, offset, size, data);
        glBindBuffer(_target, 0);
    }

    return true;
}

bool GPUBuffer::BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) {
    if(_handle == 0) return false;
    if(GetFreeSize() < size) return false;

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferSubDataEXT(_handle, _next_free_offset, size, data);
    } else {
        glBindBuffer(_target, _handle);
        glBufferSubData(_target, _next_free_offset, size, data);
        glBindBuffer(_target, 0);
    }

    *offset = _next_free_offset;

    _next_free_offset += size+1;
    _free_size -= size-1;
    _used_size += size-1;

    return true;
}

void GPUBuffer::Release(){
    if(_handle != 0){
        glDeleteBuffers(1, &_handle);
        _handle = 0;
        _allocated_size = 0;
        _next_free_offset = 0;
        _used_size = 0;
        _free_size = 0;
    }
}

bool GPUBuffer::GetBuffered(void* data, const unsigned int& offset, const unsigned int& size) {
    if(_handle == 0) return false;
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glGetNamedBufferSubDataEXT(_handle, offset, size, data);
    } else {
        glBindBuffer(_target, _handle);
        glGetBufferSubData(_target, offset, size, data);
        glBindBuffer(_target, 0);
    }
    return true;
}

bool GPUBuffer::Storage(void* initData, const unsigned int& size, const unsigned int& storageBits) {
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
        _allocated_size = size;
        _next_free_offset = 0;
        _used_size = 0;
        _free_size = size;
    }

    _allocated_size = size;
    _free_size = size;
    _used_size = 0;
    _next_free_offset = 0;

/*
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferStorageEXT(_handle, size, initData, storageBits);
    } else {
        glBindBuffer(_target, _handle);
        glBufferStorage(_target, size, initData, storageBits);
        glBindBuffer(_target, 0);
    }*/
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, initData, GL_STATIC_DRAW);
    } else {
        glBindBuffer(_target, _handle);
        glBufferData(_target, size, initData, GL_STATIC_DRAW);
        glBindBuffer(_target, 0);
    }

    return true;
}

bool GPUBuffer::_CopyTo(const unsigned int& dstHandle, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) {
    if(_handle == 0) return false;
    if(dstHandle == 0) return false;
    if(size == 0) return true;

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedCopyBufferSubDataEXT(_handle, dstHandle, srcOffset, dstOffset, size);
    } else {
        glBindBuffer(GL_COPY_READ_BUFFER, _handle);
        glBindBuffer(GL_COPY_WRITE_BUFFER, dstHandle);

        glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);

        glBindBuffer(GL_COPY_READ_BUFFER, 0);
        glBindBuffer(GL_COPY_WRITE_BUFFER, 0);
    }

    return true;
}

GPUBuffer::GPUBuffer(GL::IContext* ctx, const unsigned int& target) : Object(), IGLObject(ctx, GL::IGLObject::ObjectType::Buffer, &_handle),
_resident_ptr(0), _buffer_size(0), _usage(0), _accessFlag(0), _handle(0), _num(0), _target(target), _allocated_size(0), _used_size(0), _free_size(0), _next_free_offset(0) {
}

GPUBuffer::~GPUBuffer(){
    Release();
}

VertexBuffer::VertexBuffer(GL::IContext* ctx) : GPUBuffer(ctx, GL_ARRAY_BUFFER) {}
VertexBuffer::~VertexBuffer(){}

IndexBuffer::IndexBuffer(GL::IContext* ctx) : GPUBuffer(ctx, GL_ELEMENT_ARRAY_BUFFER) {}
IndexBuffer::~IndexBuffer(){}

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

bool GeometryBuffer::SetVertexBuffer(GPUBuffer* buf) {
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

bool GeometryBuffer::SetIndexBuffer(GPUBuffer* buf) {
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

void GeometryBuffer::Draw(IRenderSystem* rc, const unsigned int& istart, const unsigned int& vstart, const int& icount, const int& vcount) {
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
                                    vstart,
                                    vstart+vcount,
                                    icount,
                                    _iformat->GetDataType()->GPUDataType(),
                                    (void*)(_iformat->Size() * istart));
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
                glDrawArrays(_draw_mode, vstart, vcount);
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
                                    vstart,
                                    vstart+vcount,
                                    icount,
                                    _iformat->GetDataType()->GPUDataType(),
                                    (void*)(_iformat->Size() * istart));
            //}
        }
        else {
            /*if(MR::MachineInfo::Feature_DrawIndirect()) {
                glDrawArraysIndirect(_draw_mode, 0);
            } else {*/
                glDrawArrays(_draw_mode, vstart, vcount);
            //}
        }

        glBindVertexArray(0);
    }
}

void GeometryBuffer::Release() {
    if(_vao != 0) glDeleteVertexArrays(1, &_vao);
    _vao = 0;
}

GeometryBuffer::GeometryBuffer(GL::IContext* ctx, GPUBuffer* vb, GPUBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode) :
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

void Geometry::SetIStart(const unsigned int& i) {
    _istart = i;
}

void Geometry::SetVStart(const unsigned int& i) {
    _vstart = i;
}

void Geometry::SetICount(const int& i) {
    _icount = i;
}

void Geometry::SetVCount(const int& i) {
    _vcount = i;
}

void Geometry::Draw(IRenderSystem* rc){
    if(!_buffer) return;
    /*if(MR::MachineInfo::Feature_DrawIndirect()) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _draw_buffer->_handle);
    }*/

    ///MR::Log::LogString(std::to_string(_start) + " " + std::to_string(_end) + " " + std::to_string(_count));
    _buffer->Draw(rc, _istart, _vstart, _icount, _vcount);

    /*if(MR::MachineInfo::Feature_DrawIndirect()) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
    }*/
}

Geometry::Geometry(IGeometryBuffer* buffer, const unsigned int& index_start, const unsigned int& vertex_start, const int& ind_count, const int& vert_count)
 : _buffer(buffer), _istart(index_start), _vstart(vertex_start), _icount(ind_count), _vcount(vert_count) {
     //if(MR::MachineInfo::Feature_DrawIndirect()) _MakeBuffer();
}

Geometry::~Geometry(){
}

//GEOMETRY MANAGER

GPUBuffer* GeometryManager::_RequestBuffer(IVertexFormat* vfo, IIndexFormat* ifo, const size_t& data_size, const unsigned int& usage, const unsigned int& accessFlag) {
    if(!_new_buffer_per_data) {
    for(size_t i = 0; i < _buffers.size(); ++i) {
        if(
            (((vfo != 0) && (_buffers[i]->v_format != 0) && (_buffers[i]->v_format->Equal(vfo))) ||
            ((ifo != 0) && (_buffers[i]->i_format != 0) && (_buffers[i]->i_format->Equal(ifo)))) &&
            (_buffers[i]->_usage == usage) &&
            (_buffers[i]->_accessFlag == accessFlag)) {

            for(size_t i2 = 0; i2 < _buffers[i]->gl_buffers.size(); ++i2) {
                if(_buffers[i]->gl_buffers[i2]->GetFreeSize() > data_size) return (_buffers[i]->gl_buffers[i2]);
            }
        }
    }}

    //Create new buffer

    unsigned int target = 0;

    if(vfo) target = GL_ARRAY_BUFFER;
    else target = GL_ELEMENT_ARRAY_BUFFER;

    FormatBuffers* buff = new FormatBuffers();
    buff->v_format = vfo;
    buff->i_format = ifo;
    buff->_usage = usage;
    buff->_accessFlag = accessFlag;
    buff->gl_buffers.push_back(new GPUBuffer(MR::GL::GetCurrent(), target));
    if(!_new_buffer_per_data) { if(! (buff->gl_buffers[buff->gl_buffers.size()-1]->Storage(_max_buffer_size, 0) ) ) return 0; }
    else if(! (buff->gl_buffers[buff->gl_buffers.size()-1]->Storage(data_size, 0) ) ) return 0;
    _buffers.push_back(buff);

    return ((_buffers[_buffers.size()-1])->gl_buffers[(_buffers[_buffers.size()-1])->gl_buffers.size()-1]);
}

GeometryBuffer * GeometryManager::PlaceGeometry(  IVertexFormat* vfo, IIndexFormat* ifo,
                                void* vert_data, const size_t& vert_data_size,
                                void* ind_data, const size_t& ind_data_size,
                                const unsigned int& usage, const unsigned int& accessFlag, const unsigned int& drawMode,
                                unsigned int * vertex_buf_offset_in_bytes, unsigned int * index_buf_offset_in_bytes) {
    unsigned int vind = 0, iind = 0;

    GPUBuffer* vbuf = _RequestBuffer(vfo, 0, vert_data_size, usage, accessFlag);
    if(vbuf == 0) return 0;
    vbuf->BufferAutoLocate(vert_data, vert_data_size, &vind);

    GPUBuffer* ibuf = 0;
    if(ind_data_size != 0){
        ibuf = _RequestBuffer(0, ifo, ind_data_size, usage, accessFlag);
        if(ibuf == 0) return 0;
        ibuf->BufferAutoLocate(ind_data, ind_data_size, &iind);
    }

    if(vertex_buf_offset_in_bytes) *vertex_buf_offset_in_bytes = vind;
    if(index_buf_offset_in_bytes) *index_buf_offset_in_bytes = iind;

    GeometryBuffer * gb = new GeometryBuffer(GL::GetCurrent(), vbuf, ibuf, vfo, ifo, drawMode);
    return gb;
}

GeometryManager::~GeometryManager() {
    while(_buffers.size() != 0) {
        while(_buffers[_buffers.size()-1]->gl_buffers.size() != 0) {
            delete (_buffers[_buffers.size()-1]->gl_buffers[_buffers[_buffers.size()-1]->gl_buffers.size()-1]);
            _buffers[_buffers.size()-1]->gl_buffers.pop_back();
        }
        _buffers.pop_back();
    }
}

GeometryManager* __MR_GeometryManager__INSTANCE = 0;

GeometryManager* GeometryManager::Instance() {
    if(__MR_GeometryManager__INSTANCE == 0) __MR_GeometryManager__INSTANCE = new GeometryManager();
    return __MR_GeometryManager__INSTANCE;
}

void GeometryManager::DestroyInstance() {
    delete __MR_GeometryManager__INSTANCE;
    __MR_GeometryManager__INSTANCE = 0;
}

}
