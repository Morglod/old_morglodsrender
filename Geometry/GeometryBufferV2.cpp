#include "GeometryBufferV2.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Log.hpp"
#include "../Utils/Singleton.hpp"
#include "../RenderManager.hpp"
#include "../Utils/Debug.hpp"

#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

namespace MR {

void GPUBuffer::BindToStream(IGeometryStream* stream, const unsigned int& offset) {
    if(!stream) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        MR::Log::LogString("Failed GPUBuffer::BindToStream. stream is null", MR_LOG_LEVEL_ERROR);
#endif // MR_CHECK_LARGE_GL_ERRORS
        return;
    }

    if(!stream->GetBindedVertexFormat()) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        MR::Log::LogString("Failed GPUBuffer::BindToStream. stream->format is null", MR_LOG_LEVEL_ERROR);
#endif // MR_CHECK_LARGE_GL_ERRORS
        return;
    }

    glBindVertexBuffer(stream->GetGPUId(), _handle, offset, stream->GetBindedVertexFormat()->Size());
}

bool GPUBuffer::Buffer(void* data, const unsigned int& size, const unsigned int& usage){
    _usage = usage;

    if(_handle == 0) {
        glGenBuffers(1, &_handle);
#ifdef MR_GEOMETRY_DEBUG
        MR::Log::LogString("MR::GPUBuffer::Buffer("+ std::to_string((int)data) +", "+ std::to_string(size) +", "+ std::to_string(usage) +") glGenBuffer(1, &_handle)", MR_LOG_LEVEL_INFO);
#endif // MR_GEOMETRY_DEBUG
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
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, _handle);
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_buffer_size);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    return true;
}

bool GPUBuffer::BufferAutoLocate(void* data, const unsigned int& size, unsigned int * offset) {
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
        _allocated_size = size;
        _next_free_offset = 0;
        _used_size = 0;
        _free_size = size;
    }

    if(GetFreeSize() < size) return false;

    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferSubDataEXT(_handle, _next_free_offset, size, data);
    } else {
        glBindBuffer(_target, _handle);
        glBufferSubData(_target, _next_free_offset, size, data);
        glBindBuffer(_target, 0);
    }

    *offset = _next_free_offset;

    _next_free_offset += size;
    _free_size -= size;
    _used_size += size;

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

bool GPUBuffer::Update(void* data, const unsigned int& size, const unsigned int& offset) {
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

    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(_handle, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetNamedBufferParameterivEXT(_handle, GL_BUFFER_SIZE, &_buffer_size);
        } else {
            glBindBuffer(GL_ARRAY_BUFFER, _handle);
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_resident_ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_buffer_size);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

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
/*
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

    *//*if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferStorageEXT(_handle, size, initData, storageBits);
    } else {
        glBindBuffer(_target, _handle);
        glBufferStorage(_target, size, initData, storageBits);
        glBindBuffer(_target, 0);
    }*/
/*
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_handle, size, initData, GL_STATIC_DRAW);
    } else {
        glBindBuffer(_target, _handle);
        glBufferData(_target, size, initData, GL_STATIC_DRAW);
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
*/
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

GPUBuffer::GPUBuffer(const unsigned int& target) :
_resident_ptr(0), _buffer_size(0), _usage(0), _handle(0), _num(0), _target(target), _allocated_size(0), _used_size(0), _free_size(0), _next_free_offset(0) {
}

GPUBuffer::~GPUBuffer(){
    Release();
}

/*GeometryBuffer* GeometryBuffer::Plane(const glm::vec3& scale, const glm::vec3 pos, const unsigned int& usage, const unsigned int& drawm){
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
    vformat->AddVertexAttribute(new MR::VertexAttributeCustom(3, VertexDataTypeFloat::GetInstance(), IVertexAttribute::SI_Position));
    vformat->AddVertexAttribute(new MR::VertexAttributeCustom(2, VertexDataTypeFloat::GetInstance(), IVertexAttribute::SI_TexCoord));

    MR::GPUBuffer* vbuffer = new GPUBuffer(GL_ARRAY_BUFFER);
    vbuffer->Buffer(&garray[0], sizeof(float)*garray_size, usage);
    vbuffer->SetNum(4); //set vertexes num

    MR::IndexFormatCustom* iformat = new IndexFormatCustom(VertexDataTypeUInt::GetInstance());

    MR::GPUBuffer* ibuffer = new GPUBuffer(GL_ELEMENT_ARRAY_BUFFER);
    ibuffer->Buffer(&iarray[0], sizeof(unsigned int)*iarray_size, usage);
    ibuffer->SetNum(iarray_size);

    //Create geometry buffer
    MR::GeometryBuffer* gb = new GeometryBuffer(vbuffer, ibuffer, vformat, iformat, drawm); //new MR::GeometryBuffer(vDecl, iDecl, &garray[0], sizeof(float)*garray_size, &iarray[0], sizeof(unsigned int)*iarray_size, garray_size / per_verts_elements, iarray_size, usage, usage, drawm);
    delete [] iarray;

    return gb;
}*/

bool GeometryBuffer::SetVertexBuffer(GPUBuffer* buf) {
    glBindVertexArray(_vao);
    _vb = buf;
    if(buf == nullptr) {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, buf->GetGPUHandle());
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
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->GetGPUHandle());
        if(!_iformat->Bind()) return false;
    }
    glBindVertexArray(0);
    return true;
}

void GeometryBuffer::Release() {
    if(_vao != 0) glDeleteVertexArrays(1, &_vao);
    _vao = 0;
}

GeometryBuffer::GeometryBuffer(GPUBuffer* vb, GPUBuffer* ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode) :
    _vb(vb), _ib(ib), _format(f), _iformat(fi), _vao(0), _draw_mode(drawMode)
{
    if(!MR::MachineInfo::FeatureNV_GPUPTR()){
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        if(vb == nullptr) glBindBuffer(GL_ARRAY_BUFFER, 0);
        else {
            glBindBuffer(GL_ARRAY_BUFFER, vb->GetGPUHandle());
            if(f != nullptr) f->Bind();
        }

        if(ib != nullptr){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->GetGPUHandle());
            if(fi != nullptr) fi->Bind();
        }

        glBindVertexArray(0);
    }
}

GeometryBuffer::~GeometryBuffer(){
    Release();
}

void GeometryDrawParams::SetIndexStart(const unsigned int& i) {
    _istart = i;
    if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexStart(const unsigned int& v) {
    _vstart = v;
    if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

void GeometryDrawParams::SetIndexCount(const unsigned int& i) {
    _icount = i;
    if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

void GeometryDrawParams::SetVertexCount(const unsigned int& i) {
    _vcount = i;
    if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

void GeometryDrawParams::SetUseIndexBuffer(const bool& state) {
    _index_buffer = state;
    if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

bool GeometryDrawParams::GetUseIndexBuffer() {
    return _index_buffer;
}

void GeometryDrawParams::BeginDraw() {
    if(_indirect_buffer) MR::RenderManager::GetInstance()->SetIndirectDrawParamsBuffer(_indirect_buffer);
}

void GeometryDrawParams::EndDraw() {
    //MR::RenderManager::GetInstance()->SetIndirectDrawParamsBuffer(0);
}

void GeometryDrawParams::_MakeDrawCmd() {
    if(MR::MachineInfo::Feautre_DrawIndirect_UseGPUBuffer()) {
        bool direct = MR::MachineInfo::IsDirectStateAccessSupported();

        void* data = (_index_buffer == true) ? (void*)(new MR::IGeometryDrawParams::DrawElementsIndirectCmd {_icount, 1, _istart, _vstart, 0}) : (void*)(new MR::IGeometryDrawParams::DrawArraysIndirectCmd {_vcount, 1, _vstart, 0});

        if(_indirect_buffer == 0) {
            glGenBuffers(1, &_indirect_buffer);
            if(direct) glNamedBufferDataEXT(_indirect_buffer, sizeof(MR::IGeometryDrawParams::DrawElementsIndirectCmd), 0, GL_STATIC_DRAW);
            else {
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer);
                glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(MR::IGeometryDrawParams::DrawElementsIndirectCmd), 0, GL_STATIC_DRAW);
                glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
            }
        }

        if(direct) glNamedBufferSubDataEXT(_indirect_buffer, 0, ((_index_buffer == true) ? sizeof(MR::IGeometryDrawParams::DrawElementsIndirectCmd) : sizeof(MR::IGeometryDrawParams::DrawArraysIndirectCmd)), data);
        else {
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _indirect_buffer);
            glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0, ((_index_buffer == true) ? sizeof(MR::IGeometryDrawParams::DrawElementsIndirectCmd) : sizeof(MR::IGeometryDrawParams::DrawArraysIndirectCmd)), data);
            glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
        }

        if(_index_buffer) delete ((MR::IGeometryDrawParams::DrawElementsIndirectCmd*)data);
        else delete ((MR::IGeometryDrawParams::DrawArraysIndirectCmd*)data);
    }
    else {
        if(_index_buffer) {
            if(drawCmd) {
                delete ((MR::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
            }
            drawCmd = new MR::IGeometryDrawParams::DrawElementsIndirectCmd {_icount, 1, _istart, _vstart, 0};
        } else {
            if(drawCmd) {
                delete ((MR::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
            }
            drawCmd = new MR::IGeometryDrawParams::DrawArraysIndirectCmd {_vcount, 1, _vstart, 0};
        }
    }
}

GeometryDrawParams::GeometryDrawParams(const bool& indexBuffer, const unsigned int& istart, const unsigned int& vstart, const unsigned int& icount, const unsigned int& vcount)
 : drawCmd(0), _istart(istart), _vstart(vstart), _icount(icount), _vcount(vcount), _index_buffer(indexBuffer) {
     if(MR::MachineInfo::Feature_DrawIndirect()) _MakeDrawCmd();
}

GeometryDrawParams::~GeometryDrawParams() {
    EndDraw();

    if(_indirect_buffer != 0){
        glDeleteBuffers(1, &_indirect_buffer);
    }
    else if(drawCmd) {
        if(_index_buffer) {
            delete ((MR::IGeometryDrawParams::DrawElementsIndirectCmd*)drawCmd);
        } else {
            delete ((MR::IGeometryDrawParams::DrawArraysIndirectCmd*)drawCmd);
        }
        drawCmd = 0;
    }
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawArrays(const unsigned int& vstart, const unsigned int& vcount) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(false, 0, vstart, 0, vcount));
}

IGeometryDrawParamsPtr GeometryDrawParams::DrawElements(const unsigned int& istart, const unsigned int& icount) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(true, istart, 0, icount, 0));
}

void Geometry::SetGeometryBuffer(IGeometryBuffer* buffer) {
    _buffer = buffer;
}

void Geometry::Draw(){
    if(!_buffer || !_draw_params) return;
    MR::RenderManager::GetInstance()->DrawGeometryBuffer(_buffer, _draw_params);

    /*_draw_params->BeginDraw();
    if(MR::MachineInfo::Feature_DrawIndirect()) _buffer->Draw(_draw_params->GetIndirectPtr());
    else _buffer->Draw(_draw_params->GetIndexStart(), _draw_params->GetVertexStart(), _draw_params->GetIndexCount(), _draw_params->GetVertexCount());
    _draw_params->EndDraw();*/
}

Geometry::Geometry(IGeometryBuffer* buffer, IGeometryDrawParamsPtr params)
 : _buffer(buffer), _draw_params(params) {
}

Geometry::~Geometry(){
}

//DEFAULT GEOMETRY SHAPES
Geometry* Geometry::MakeTriangle(const float& scale, const glm::vec3& offset) {
    MR::VertexFormatCustomFixed* vformat = new MR::VertexFormatCustomFixed();
    vformat->SetAttributesNum(1);
    vformat->AddVertexAttribute(new MR::VertexAttributePos3F());

    float vdata[] {
        offset.x,                   offset.y + 1.0f * scale, offset.z,
        offset.x + 1.0f * scale,    offset.y - 1.0f * scale, offset.z,
        offset.x - 1.0f * scale,    offset.y - 1.0f * scale, offset.z
    };

    IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, nullptr, &vdata[0], sizeof(float) * 9, nullptr, 0, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawArrays(0, 3));
    return geom;
}

Geometry* Geometry::MakeQuad(const glm::vec2& scale, const glm::vec3& offset, const bool& texCoords, const glm::vec2& texCoordsScale) {
    MR::VertexFormatCustomFixed* vformat = new MR::VertexFormatCustomFixed();
    vformat->SetAttributesNum(texCoords ? 2 : 1);
    vformat->AddVertexAttribute(MR::VertexAttributePos3F::GetInstance());
    if(texCoords) vformat->AddVertexAttribute(new MR::VertexAttributeCustom(2, MR::VertexDataTypeFloat::GetInstance(), MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION));

    MR::IndexFormatCustom* iformat = new MR::IndexFormatCustom(new VertexDataTypeCustom(GL_UNSIGNED_BYTE, 1));

    float vdata_noTexCoords[] {
        offset.x,                   offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z
    };

    float vdata_texCoords[] {
        offset.x,                   offset.y,                 offset.z, //p
        0.0f, 1.0f * texCoordsScale.y,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z, //p
        1.0f * texCoordsScale.x, 1.0f * texCoordsScale.y,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z, //p
        1.0f * texCoordsScale.x, 0.0f,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z,  //p
        0.0f, 0.0f
    };

    float* vdata = texCoords ? &vdata_texCoords[0] : &vdata_noTexCoords[0];

    unsigned char idata[] {
        0, 1, 2,
        2, 3, 0
    };

    IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * (texCoords ? 20 : 12), &idata[0], sizeof(unsigned char) * 6, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 6));
    return geom;
}

Geometry* Geometry::MakeBox(const glm::vec3& scale, const glm::vec3& offset, const bool& inside) {
    MR::VertexFormatCustomFixed* vformat = new MR::VertexFormatCustomFixed();
    vformat->SetAttributesNum(1);
    vformat->AddVertexAttribute(new MR::VertexAttributePos3F());

    MR::IndexFormatCustom* iformat = new MR::IndexFormatCustom(new VertexDataTypeCustom(GL_UNSIGNED_BYTE, 1));

    float vdata[] {
        //front face
        offset.x,                   offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z,

        //back face
        offset.x,                   offset.y,                 offset.z + 1.0f * scale.z,
        offset.x + 1.0f * scale.x,  offset.y,                 offset.z + 1.0f * scale.z,
        offset.x + 1.0f * scale.x,  offset.y + 1.0f * scale.y,offset.z + 1.0f * scale.z,
        offset.x,                   offset.y + 1.0f * scale.y,offset.z + 1.0f * scale.z
    };

    unsigned char idata_normal[] {
        2, 1, 0,
        0, 3, 2,

        6, 5, 1,
        1, 2, 6,

        3, 0, 4,
        4, 7, 3,

        6, 2, 3,
        3, 7, 6,

        7, 4, 5,
        5, 6, 7,

        1, 5, 4,
        4, 0, 1
    };

    unsigned char idata_reversed[] {
        0, 1, 2,
        2, 3, 0,

        1, 5, 6,
        6, 2, 1,

        4, 0, 3,
        3, 7, 4,

        3, 2, 6,
        6, 7, 3,

        5, 4, 7,
        7, 6, 5,

        4, 5, 1,
        1, 0, 4
    };

    unsigned char* idata = (inside) ? &idata_reversed[0] : &idata_normal[0];

    IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * 24, &idata[0], sizeof(unsigned char) * 36, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 36));
    return geom;
}

//GEOMETRY MANAGER

GPUBuffer* GeometryManager::_RequestVBuffer(void* data, IVertexFormat* vfo, const size_t& data_size, const unsigned int& usage, const unsigned int& accessFlag, bool * savedToBuffer) {
    if(!_new_buffer_per_data) { ///TODO set to (!_new_buffer_per_data)
        for(size_t i = 0; i < _buffers.size(); ++i) {
            if(
                ((vfo != 0) && (_buffers[i]->v_format != 0) && (_buffers[i]->v_format->Equal(vfo))) &&
                (_buffers[i]->_usage == usage) &&
                (_buffers[i]->_accessFlag == accessFlag)) {

                for(size_t i2 = 0; i2 < _buffers[i]->gl_buffers.size(); ++i2) {
                    if(_buffers[i]->gl_buffers[i2]->GetFreeSize() > data_size) {
                        *savedToBuffer = false;
                        return (_buffers[i]->gl_buffers[i2]);
                    }
                }
            }
        }
    }

    //Create new buffer
    FormatBuffers* buff = new FormatBuffers();
    buff->v_format = vfo;
    buff->_usage = usage;
    buff->_accessFlag = accessFlag;
    buff->gl_buffers.push_back(new GPUBuffer(GL_ARRAY_BUFFER));

    if(!_new_buffer_per_data) {
        if(! (buff->gl_buffers[buff->gl_buffers.size()-1]->Buffer(0, _max_buffer_size, usage) ) ) {
            return 0;
        }
    }
    else if(! (buff->gl_buffers[buff->gl_buffers.size()-1]->Buffer(data, data_size, usage) ) ) {
        *savedToBuffer = false;
        return 0;
    }

    _buffers.push_back(buff);

    if(_new_buffer_per_data) *savedToBuffer = true;
    else  *savedToBuffer = false;

    return ((_buffers[_buffers.size()-1])->gl_buffers[(_buffers[_buffers.size()-1])->gl_buffers.size()-1]);
}

GeometryBuffer * GeometryManager::PlaceGeometry(  IVertexFormat* vfo, IIndexFormat* ifo,
                                void* vert_data, const size_t& vert_data_size,
                                void* ind_data, const size_t& ind_data_size,
                                const unsigned int& usage, const unsigned int& accessFlag, const unsigned int& drawMode,
                                unsigned int * vertex_buf_offset_in_bytes, unsigned int * index_buf_offset_in_bytes) {
    unsigned int vind = 0, iind = 0;
    bool saved = false;

    GPUBuffer* vbuf = _RequestVBuffer(vert_data, vfo, vert_data_size, usage, accessFlag, &saved);
    if(vbuf == 0) return 0;
    if(!saved) vbuf->BufferAutoLocate(vert_data, vert_data_size, &vind);

    GPUBuffer* ibuf = 0;
    if(ind_data_size != 0){
        ibuf = new GPUBuffer(GL_ELEMENT_ARRAY_BUFFER);
        if(ibuf == 0) return 0;
        ibuf->Buffer(ind_data, ind_data_size, usage);
    }

    if(vertex_buf_offset_in_bytes) *vertex_buf_offset_in_bytes = vind;
    if(index_buf_offset_in_bytes) *index_buf_offset_in_bytes = iind;

    GeometryBuffer * gb = new GeometryBuffer(vbuf, ibuf, vfo, ifo, drawMode);
    return gb;
}

GeometryManager::GeometryManager() : _max_buffer_size(104857600), _new_buffer_per_data(true) {
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

}
