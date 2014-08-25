#include "GeometryBufferV2.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Log.hpp"
#include "../Utils/Singleton.hpp"
#include "../RenderManager.hpp"
#include "../Utils/Debug.hpp"
#include "../Buffers/Buffers.hpp"

#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

namespace MR {

void GPUGeometryBuffer::SetGPUBuffer(IGPUBuffer* buf) {
    Assert(!buf)

    if(MR::MachineInfo::FeatureNV_GPUPTR()){
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glGetNamedBufferParameterui64vNV(buf->GetGPUHandle(), GL_BUFFER_GPU_ADDRESS_NV, &_nv_resident_ptr);
            glGetNamedBufferParameterivEXT(buf->GetGPUHandle(), GL_BUFFER_SIZE, &_nv_buffer_size);
        } else {
            IGPUBuffer* binded = buf->ReBind(MR::IGPUBuffer::ArrayBuffer);
            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_nv_resident_ptr);
            glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &_nv_buffer_size);
            if(binded) binded->Bind(MR::IGPUBuffer::ArrayBuffer);
        }
    }

    _buffer = buf;
}

void GPUGeometryBuffer::GetNVGPUPTR(uint64_t* nv_resident_ptr, int* nv_buffer_size) {
    Assert(!nv_resident_ptr)
    Assert(!nv_buffer_size)

    *nv_resident_ptr = _nv_resident_ptr;
    *nv_buffer_size = _nv_buffer_size;
}

GPUGeometryBuffer::GPUGeometryBuffer(IGPUBuffer* buf) : _buffer(nullptr), _nv_resident_ptr(0), _nv_buffer_size(0) {
    SetGPUBuffer(buf);
}

GPUGeometryBuffer::~GPUGeometryBuffer() {
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

bool GeometryBuffer::SetVertexBuffer(IGPUGeometryBufferPtr buf) {
    _vb = buf;
    if(_vao) {
        glBindVertexArray(_vao);
        if(buf == nullptr) {
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, buf->GetGPUBuffer()->GetGPUHandle());
            if(!_format->Bind()) return false;
        }
        glBindVertexArray(0);
    }
    return true;
}

bool GeometryBuffer::SetIndexBuffer(IGPUGeometryBufferPtr buf) {
    _ib = buf;
    if(_vao) {
        glBindVertexArray(_vao);
        if(buf == nullptr) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        } else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buf->GetGPUBuffer()->GetGPUHandle());
            if(!_iformat->Bind()) return false;
        }
        glBindVertexArray(0);
    }
    return true;
}

void GeometryBuffer::Release() {
    if(_vao != 0) glDeleteVertexArrays(1, &_vao);
    _vao = 0;
}

GeometryBuffer::GeometryBuffer(IGPUGeometryBufferPtr vb, IGPUGeometryBufferPtr ib, IVertexFormat* f, IIndexFormat* fi, const unsigned int& drawMode) :
    _vb(vb), _ib(ib), _format(f), _iformat(fi), _vao(0), _draw_mode(drawMode)
{
    if(!MR::MachineInfo::FeatureNV_GPUPTR()){
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);

        if(vb == nullptr) glBindBuffer(GL_ARRAY_BUFFER, 0);
        else {
            glBindBuffer(GL_ARRAY_BUFFER, vb->GetGPUBuffer()->GetGPUHandle());
            if(f != nullptr) f->Bind();
        }

        if(ib != nullptr){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->GetGPUBuffer()->GetGPUHandle());
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

IGeometryDrawParamsPtr GeometryDrawParams::DrawElements(const unsigned int& istart, const unsigned int& icount, const unsigned int& vstart) {
    return IGeometryDrawParamsPtr(new GeometryDrawParams(true, istart, vstart, icount, 0));
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
IGeometry* Geometry::MakeTriangle(const float& scale, const glm::vec3& offset) {
    MR::VertexFormatCustomFixed* vformat = new MR::VertexFormatCustomFixed();
    vformat->SetAttributesNum(1);
    vformat->AddVertexAttribute(new MR::VertexAttributePos3F());

    float vdata[] {
        offset.x,                   offset.y + 1.0f * scale, offset.z,
        offset.x + 1.0f * scale,    offset.y - 1.0f * scale, offset.z,
        offset.x - 1.0f * scale,    offset.y - 1.0f * scale, offset.z
    };

    return MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, &vdata[0], 3,
                                                                                nullptr, 0, 0,
                                                                                IGPUBuffer::Static, GL_TRIANGLES);
}

IGeometry* Geometry::MakeQuad(const glm::vec2& scale, const glm::vec3& offset, const bool& texCoords, const glm::vec2& texCoordsScale) {
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

    //IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * (texCoords ? 20 : 12), &idata[0], sizeof(unsigned char) * 6, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    //Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 6));
    //return geom;
    return MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, &vdata[0], 4,
                                                            iformat, &idata[0], 6,
                                                            IGPUBuffer::Static, GL_TRIANGLES);
}

IGeometry* Geometry::MakeBox(const glm::vec3& scale, const glm::vec3& offset, const bool& inside) {
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

    //IGeometryBuffer* buffer = MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, iformat, &vdata[0], sizeof(float) * 24, &idata[0], sizeof(unsigned char) * 36, GL_STATIC_DRAW, GL_READ_ONLY, GL_TRIANGLES, nullptr, nullptr);
    //Geometry* geom = new MR::Geometry(buffer, GeometryDrawParams::DrawElements(0, 36));
    return MR::GeometryManager::GetInstance()->PlaceGeometry(vformat, &vdata[0], 8,
                                                            iformat, &idata[0], 36,
                                                            IGPUBuffer::Static, GL_TRIANGLES);
}

//GEOMETRY MANAGER

IGeometry* GeometryManager::PlaceGeometry(IVertexFormat* vertexFormat, void* vertexData, const size_t& vertexNum,
                             IIndexFormat* indexFormat, void* indexData, const size_t& indexNum,
                             const IGPUBuffer::Usage& usage, const unsigned int& drawMode) {
    Assert(!vertexFormat)
    Assert(!vertexData)
    Assert(vertexNum == 0)

    if(indexFormat){
        Assert(!indexData)
        Assert(indexNum == 0)
    }

    const size_t vertexDataSize = vertexNum * vertexFormat->Size();
    const size_t indexDataSize = (indexFormat) ? (indexNum * indexFormat->Size()) : 0;

    if(_buffer_per_geom) {
        GPUBuffer* vertexBuffer = new MR::GPUBuffer();
        vertexBuffer->Allocate(usage, vertexDataSize, false);
        vertexBuffer->BufferData(vertexData, 0, vertexDataSize, nullptr, nullptr);
        GPUGeometryBuffer* vertexGeom = new MR::GPUGeometryBuffer(vertexBuffer);

        GPUBuffer* indexBuffer = nullptr;
        GPUGeometryBuffer* indexGeom = nullptr;
        if(indexFormat) {
            indexBuffer = new MR::GPUBuffer();
            indexBuffer->Allocate(usage, indexDataSize, false);
            indexBuffer->BufferData(indexData, 0, indexDataSize, nullptr, nullptr);
            indexGeom = new MR::GPUGeometryBuffer(indexBuffer);
        }
        MR::GeometryBuffer* geomBuffer = new MR::GeometryBuffer(IGPUGeometryBufferPtr(vertexGeom), (indexFormat) ? IGPUGeometryBufferPtr(indexGeom) : nullptr, vertexFormat, indexFormat, drawMode);
        return dynamic_cast<MR::IGeometry*>(new MR::Geometry(dynamic_cast<MR::IGeometryBuffer*>(geomBuffer), (indexFormat) ? GeometryDrawParams::DrawElements(0, indexDataSize / indexFormat->Size(), 0) : GeometryDrawParams::DrawArrays(0, vertexDataSize/vertexFormat->Size()) ));
    }

    FormatBuffer* fbuf = _RequestFormatBuffer(vertexFormat, vertexDataSize, indexFormat, indexDataSize, usage);

    VirtualGPUBuffer* virtualBuffer = fbuf->manager->Take(vertexDataSize+indexDataSize);
    unsigned char* mappedMem = (unsigned char*)(virtualBuffer->GetMappedMemory());
    MR::IGPUBuffer::BufferedDataInfo bufferedVertexDataInfo, bufferedIndexDataInfo;
    if(mappedMem != 0) {
        for(size_t mi = 0; mi < vertexDataSize; ++mi) {
            mappedMem[mi] = ((unsigned char*)(vertexData))[mi];
        }
        if(indexDataSize) {
            for(size_t mi = vertexDataSize; mi < (vertexDataSize+indexDataSize); ++mi) {
                mappedMem[mi] = ((unsigned char*)(indexData))[mi-vertexDataSize];
            }
        }
    } else {
        virtualBuffer->BufferData(vertexData, 0, vertexDataSize, nullptr, &bufferedVertexDataInfo);
        if(indexDataSize) virtualBuffer->BufferData(indexData, vertexDataSize, indexDataSize, nullptr, &bufferedIndexDataInfo);
    }

    GPUGeometryBuffer* vertexGeom = new GPUGeometryBuffer(fbuf->manager->GetRealBuffer());
    GPUGeometryBuffer* indexGeom = (indexFormat) ? (new GPUGeometryBuffer(fbuf->manager->GetRealBuffer())) : nullptr;
    MR::GeometryBuffer* geomBuffer = new MR::GeometryBuffer(IGPUGeometryBufferPtr(vertexGeom), (indexFormat) ? IGPUGeometryBufferPtr(indexGeom) : nullptr, vertexFormat, indexFormat, drawMode);
    return dynamic_cast<MR::IGeometry*>(new MR::Geometry(dynamic_cast<MR::IGeometryBuffer*>(geomBuffer), (indexFormat) ? GeometryDrawParams::DrawElements(bufferedIndexDataInfo.offset / indexFormat->Size(), indexNum, bufferedVertexDataInfo.offset / vertexFormat->Size()) : GeometryDrawParams::DrawArrays(bufferedVertexDataInfo.offset / vertexFormat->Size(), vertexNum) ));
}

GeometryManager::FormatBuffer* GeometryManager::_RequestFormatBuffer(IVertexFormat* vertexFormat, const size_t& vertexDataSize,
                             IIndexFormat* indexFormat, const size_t& indexDataSize,
                             const IGPUBuffer::Usage& usage) {

    for(size_t i = 0; i < _buffers.size(); ++i) {
        if((_buffers[i].vFormat && _buffers[i].vFormat->Equal(vertexFormat)) &&
           (_buffers[i].iFormat && _buffers[i].iFormat->Equal(indexFormat)) &&
           (_buffers[i].usage == usage) &&
           (_buffers[i].manager))
        {
            if(_buffers[i].manager->GetFreeMemorySize() >= vertexDataSize+indexDataSize) {
                return &_buffers[i];
            }
        }
    }

    FormatBuffer formatBuf;
    formatBuf.buffer = new MR::GPUBuffer();
    formatBuf.buffer->Allocate(usage, std::max(vertexDataSize+indexDataSize, _max_buffer_size), _map_global_buffers);
    formatBuf.iFormat = indexFormat;
    formatBuf.vFormat = vertexFormat;
    formatBuf.usage = usage;
    formatBuf.manager = new MR::VirtualGPUBufferManager(formatBuf.buffer, 0);
    _buffers.push_back(formatBuf);

    return &_buffers[_buffers.size()-1];
}

//5 mb per buffer
GeometryManager::GeometryManager() : _max_buffer_size(5242880), _buffer_per_geom(false), _map_global_buffers(false) {
}

GeometryManager::~GeometryManager() {
    while(!_buffers.empty()) {
        FormatBuffer fbuf = _buffers.back();
        if(fbuf.manager) {
            delete fbuf.manager;
        }
        if(fbuf.buffer) {
            fbuf.buffer->Destroy();
            delete fbuf.buffer;
        }
        _buffers.pop_back();
    }
}

}
