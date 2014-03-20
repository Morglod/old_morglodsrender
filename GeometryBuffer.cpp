#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "Log.hpp"
#include "Transformation.hpp"
#include "MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

VertexDeclarationType::VertexDeclarationType() : _pointer(nullptr), _type(VertexDeclarationType::DataType::Position) {
}

VertexDeclarationType::VertexDeclarationType(const VertexDeclarationType::DataType& t, const GLvoid *p) : _pointer(p), _type(t) {
}

VertexDeclarationType* VertexDeclaration::AddType(const VertexDeclarationType::DataType& t, const void *p){
    if(_decl_types_num < 4){
        _decl_types[_decl_types_num] = VertexDeclarationType(t, p);
        ++_decl_types_num;
        return &_decl_types[_decl_types_num-1];
    }
    return nullptr;
}

void VertexDeclaration::ClearTypesList(){
    _decl_types_num = 0;
}

VertexDeclaration::VertexDeclaration(const DataType& dtype) : _decl_types( (VertexDeclarationType*)malloc(sizeof(VertexDeclarationType)*4) ){
}

VertexDeclaration::VertexDeclaration(VertexDeclarationType* vdt, const unsigned short& dn, const DataType& dt) {
    this->_decl_types = vdt;
    this->_decl_types_num = dn;
    this->_data_type = dt;

    switch( (int)dt ) {
    case GL_INT:
        this->_stride_size = sizeof(int);
        break;
    case GL_SHORT:
        this->_stride_size = sizeof(short);
        break;
    case GL_FLOAT:
        this->_stride_size = sizeof(float);
        break;
    case GL_DOUBLE:
        this->_stride_size = sizeof(double);
        break;
    default:
        this->_stride_size = sizeof(float);
        break;
    }

    unsigned int e_num = 0;
    for(unsigned short i = 0; i < dn; ++i) {
        e_num += _decl_types[i].ElementsNum();
    }
    this->_stride_size *= e_num;
}

VertexDeclaration::~VertexDeclaration() {
    delete [] this->_decl_types;
    this->_decl_types = nullptr;
}

IndexDeclaration::IndexDeclaration(const DataType& dt) : _data_type(dt) {}

void InstancedDataType::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, _buffer);
    glVertexAttribPointer(_shader_location, _size, (unsigned int)_type, _normalized, _stride, _pointer);
    glEnableVertexAttribArray(_shader_location);
    glVertexAttribDivisor(_shader_location, _every_vertexes);
}

void InstancedDataType::Unbind()  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedDataType::BufferData(void* data, const int& data_size, const unsigned int& usage) {
    _size = data_size;
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_buffer, data_size, data, usage);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

InstancedDataType::InstancedDataType(void* data, const int& data_size, const unsigned int& usage) : _size(data_size) {
    glGenBuffers(1, &_buffer);
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glNamedBufferDataEXT(_buffer, data_size, data, usage);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _buffer);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void InstancedDataBuffer::Bind() {
    for(unsigned short it = 0; it < _data_types_num; ++it) {
        _data_types[it].Bind();
    }
}

void InstancedDataBuffer::Unbind() {
    for(unsigned short it = 0; it < _data_types_num; ++it) {
        _data_types[it].Unbind();
    }
}

void GeometryBuffer::Bind() {
    glBindVertexArray(_vao);
    if(_idecl != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
    if( (instDataBuffer != nullptr) && (bindInstDataBuffer) ) instDataBuffer->Bind();
}

void GeometryBuffer::Unbind() {
    if( (instDataBuffer != nullptr) && (bindInstDataBuffer) ) instDataBuffer->Unbind();
    if(_idecl != nullptr) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void GeometryBuffer::Draw() {
    if( (instDataBuffer != nullptr) && (drawInstDataBuffer) ) {
        if(_idecl == nullptr) glDrawArraysInstanced((unsigned int)_draw_mode, 0, _vertexes_num, instDataBuffer->_instances_num);
        else glDrawElementsInstanced((unsigned int)_draw_mode, _indexes_num, (unsigned int)_idecl->_data_type, (void*)0, instDataBuffer->_instances_num);
    } else {
        if(_idecl == nullptr) glDrawArrays((unsigned int)_draw_mode, 0, _vertexes_num);
        else glDrawRangeElements((unsigned int)_draw_mode, 0, _indexes_num, _indexes_num, (unsigned int)_idecl->_data_type, (void*)0);
    }
}

void GeometryBuffer::SetInstData(InstancedDataBuffer* idb) {
    if(instDataBuffer != idb) {
        instDataBuffer = idb;
        OnInstDataBufferChanged(this, idb);
    }
}

void GeometryBuffer::SetDrawMode(const DrawMode& dm) {
    if(_draw_mode != dm) {
        _draw_mode = dm;
        OnDrawModeChanged(this, dm);
    }
}

void GeometryBuffer::CalcRadius(float* data) {
    float point[3] {0.0f,0.0f,0.0f};

    unsigned short pos_offset = 0;
    unsigned short stride = 0;

    if(_vdecl == nullptr) {
        _radius = 0.0f;
        return;
    }
    for(unsigned short it = 0; it < _vdecl->_decl_types_num; ++it) {
        if(_vdecl->_decl_types[it]._type == VertexDeclarationType::DataType::Position) {
            break;
        }
        pos_offset += _vdecl->_decl_types[it].ElementsNum();
    }
    for(unsigned short it = 0; it < _vdecl->_decl_types_num; ++it) {
        stride += _vdecl->_decl_types[it].ElementsNum();
    }

    for(unsigned int it = pos_offset; it < (_vertexes_num*stride); it += stride ) {
        point[0] = std::max( point[0], std::abs(data[it  ]) );
        point[1] = std::max( point[1], std::abs(data[it+1]) );
        point[2] = std::max( point[2], std::abs(data[it+2]) );
    }

    _radius = glm::length( glm::vec3(point[0], point[1], point[2]) );
}

void GeometryBuffer::CalcRadius() {
    unsigned char* data = new unsigned char[_data_size];
    GetVData(data);
    CalcRadius((float*)data);
    delete [] data;
}

void GeometryBuffer::GetVData(void* da) {
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glGetNamedBufferSubDataEXT(_vertex_buffer, 0, _data_size, da);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
        glGetBufferSubData(	GL_ARRAY_BUFFER, 0, _data_size, da);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void GeometryBuffer::GetIData(void* da) {
    if(MR::MachineInfo::IsDirectStateAccessSupported()){
        glGetNamedBufferSubDataEXT(_index_buffer, 0, _data_size, da);
    } else {
        glBindBuffer(GL_ARRAY_BUFFER, _index_buffer);
        glGetBufferSubData(	GL_ARRAY_BUFFER, 0, _data_size, da);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

GeometryBuffer* GeometryBuffer::Copy() {
    VertexDeclarationType vdtc[this->_vdecl->_decl_types_num];
    for(unsigned short it = 0; it < this->_vdecl->_decl_types_num; ++it) {
        vdtc[it] = VertexDeclarationType(this->_vdecl->_decl_types[it]._type, this->_vdecl->_decl_types[it]._pointer);
    }
    VertexDeclaration* vdc = new VertexDeclaration(&vdtc[0], this->_vdecl->_decl_types_num, this->_vdecl->_data_type);

    IndexDeclaration* idc = new IndexDeclaration(this->_idecl->_data_type);

    float* vdata = new float[this->_data_size];
    GetVData(vdata);

    unsigned int * idata = new unsigned int[this->_idata_size];
    GetIData(idata);

    return (new GeometryBuffer(vdc, idc, vdata, this->_data_size, idata, _idata_size, _vertexes_num, _indexes_num, _data_usage, _idata_usage, _draw_mode));
}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, const DrawMode& drawm, bool calcR) {
    this->_draw_mode = drawm;
    this->_vdecl = vd;
    this->_idecl = id;
    this->_vertex_buffer = vb;
    this->_index_buffer = ib;
    this->_vertexes_num = vnum;
    this->_indexes_num = inum;
    if(calcR) CalcRadius();
}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, const Usage& usage, const Usage& iusage, const DrawMode& drawm, bool calcR)
    : _vdecl(vd), _idecl(id), _vertexes_num(vnum), _indexes_num(inum), _draw_mode(drawm), _data_size(data_size), _idata_size(idata_size), _data_usage(usage), _idata_usage(iusage) {
    //Create vertex array
    glGenVertexArrays(1, &_vao);
    if(!MR::MachineInfo::IsDirectStateAccessSupported()) glBindVertexArray(_vao);

    //Create data buffer
    glGenBuffers(1, &_vertex_buffer);
    if(!MR::MachineInfo::IsDirectStateAccessSupported()) glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);

    if(MR::MachineInfo::IsDirectStateAccessSupported()) glNamedBufferDataEXT(_vertex_buffer, data_size, data, (unsigned int)usage);
    else glBufferData(GL_ARRAY_BUFFER, data_size, data, (unsigned int)usage);

    //Point attributes

    VertexDeclarationType* v_pos = _vdecl->Get(VertexDeclarationType::DataType::Position);
    if(v_pos != nullptr) {
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glVertexArrayVertexAttribOffsetEXT(_vao, _vertex_buffer, MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, v_pos->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, (int)v_pos->_pointer);
            glEnableVertexArrayAttribEXT(_vao, MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION);
        } else {
            glVertexAttribPointer(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, v_pos->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, v_pos->_pointer);
            glEnableVertexAttribArray(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION);
        }
    }

    VertexDeclarationType* v_texcoord = _vdecl->Get(VertexDeclarationType::DataType::TexCoord);
    if(v_texcoord != nullptr) {
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glVertexArrayVertexAttribOffsetEXT(_vao, _vertex_buffer, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, v_texcoord->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, (int)v_texcoord->_pointer);
            glEnableVertexArrayAttribEXT(_vao, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION);
        } else {
            glVertexAttribPointer(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, v_texcoord->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, v_texcoord->_pointer);
            glEnableVertexAttribArray(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION);
        }
    }

    VertexDeclarationType* v_norm = _vdecl->Get(VertexDeclarationType::DataType::Normal);
    if(v_norm != nullptr) {
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glVertexArrayVertexAttribOffsetEXT(_vao, _vertex_buffer, MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, v_norm->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, (int)v_norm->_pointer);
            glEnableVertexArrayAttribEXT(_vao, MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION);
        } else {
            glVertexAttribPointer(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, v_norm->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, v_norm->_pointer);
            glEnableVertexAttribArray(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION);
        }
    }

    VertexDeclarationType* v_color = _vdecl->Get(VertexDeclarationType::DataType::Color);
    if(v_color != nullptr) {
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glVertexArrayVertexAttribOffsetEXT(_vao, _vertex_buffer, MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION, v_color->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, (int)v_color->_pointer);
            glEnableVertexArrayAttribEXT(_vao, MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION);
        } else {
            glVertexAttribPointer(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION, v_color->ElementsNum(), (unsigned int)_vdecl->_data_type, GL_FALSE, _vdecl->_stride_size, v_color->_pointer);
            glEnableVertexAttribArray(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION);
        }
    }

    //Unbind vertex array
    if(!MR::MachineInfo::IsDirectStateAccessSupported()) glBindVertexArray(0);

    //Create index buffer
    if(_idecl != nullptr) {
        glGenBuffers(1, &this->_index_buffer);
        if(MR::MachineInfo::IsDirectStateAccessSupported()){
            glNamedBufferDataEXT(_index_buffer, idata_size, idata, (unsigned int)iusage);
        } else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, idata_size, idata, (unsigned int)iusage);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
    }

    if(calcR) CalcRadius((float*)data);
}

GeometryBuffer::~GeometryBuffer() {
    if(this->_vertex_buffer != 0) {
        glBindBuffer(GL_VERTEX_ARRAY, 0);
        glDeleteBuffers(1, &this->_vertex_buffer);
    }
    if(this->_index_buffer != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glDeleteBuffers(1, &this->_index_buffer);
    }
    if(this->_vao != 0) {
        glBindVertexArray(0);
        glDeleteVertexArrays(1, &this->_vao);
    }
}

GeometryBuffer* GeometryBuffer::CreatePlane(const glm::vec3& scale, const glm::vec3 pos, const Usage& usage, const DrawMode& drawm){
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

    switch((unsigned int)drawm){
    case (unsigned int)GeometryBuffer::DrawMode::Points:
    case (unsigned int)GeometryBuffer::DrawMode::LineLoop:
    case (unsigned int)GeometryBuffer::DrawMode::LineStrip:
    case (unsigned int)GeometryBuffer::DrawMode::Quads:
        iarray = new unsigned int[4] { 0, 1, 2, 3 };
        iarray_size = 4;
        break;
    case (unsigned int)GeometryBuffer::DrawMode::Triangles:
        iarray = new unsigned int[6] { 0, 1, 2, 2, 3, 0 };
        iarray_size = 6;
        break;
    }

    //Create vertex declaration that tells engine in wich sequence data is stored
    MR::VertexDeclarationType vdtypes[2] {
        MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Position, 0),
        MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::TexCoord, (void*)(sizeof(float)* 3))
    };

    MR::VertexDeclaration* vDecl = new MR::VertexDeclaration(&vdtypes[0], 2, MR::VertexDeclaration::DataType::Float);
    MR::IndexDeclaration* iDecl = new MR::IndexDeclaration(MR::IndexDeclaration::DataType::UInt);

    //Create geometry buffer
    MR::GeometryBuffer* gb = new MR::GeometryBuffer(vDecl, iDecl, &garray[0], sizeof(float)*garray_size, &iarray[0], sizeof(unsigned int)*iarray_size, garray_size / per_verts_elements, iarray_size, usage, usage, drawm);
    delete [] iarray;

    return gb;
}

GeometryBuffer* GeometryBuffer::CreateCube(const glm::vec3& scale, const glm::vec3 pos, const Usage& usage, const DrawMode& drawm){
    const int per_verts_elements = 5;
    const int garray_size = per_verts_elements * 16;
    float garray[garray_size] {
        //v0
        -0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z, //pos
        0.0f, 1.0f, //tex coord
        //v1
        -0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        0.0f, 0.0f,
        //v2
         0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        1.0f, 0.0f,
        //v3
         0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        1.0f, 1.0f,
        //v4
        -0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        0.0f, 1.0f,
        //v5
        -0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        0.0f, 0.0f,
        //v6
         0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        1.0f, 0.0f,
        //v7
         0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        1.0f, 1.0f,

        //sides

        //v8 0
        -0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z, //pos
        0.0f, 1.0f, //tex coord
        //v9 1
        -0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        0.0f, 0.0f,
        //v10 2
         0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        1.0f, 0.0f,
        //v11 3
         0.5f*scale.x + pos.x,-0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        1.0f, 1.0f,
        //v12 4
        -0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        0.0f, 1.0f,
        //v13 5
        -0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        0.0f, 0.0f,
        //v14 6
         0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y, 0.5f*scale.z + pos.z,
        1.0f, 0.0f,
        //v15 7
         0.5f*scale.x + pos.x, 0.5f*scale.y + pos.y,-0.5f*scale.z + pos.z,
        1.0f, 1.0f
    };

    //Create index array for 2 triangles

    int iarray_size = 24;
    unsigned int* iarray = 0;

    switch((unsigned int)drawm){
    case (unsigned int)GeometryBuffer::DrawMode::Points:
    case (unsigned int)GeometryBuffer::DrawMode::LineLoop:
    case (unsigned int)GeometryBuffer::DrawMode::LineStrip:
    case (unsigned int)GeometryBuffer::DrawMode::Quads:
        iarray = new unsigned int[8] {
            0, 1, 2, 3, //bot
            4, 5, 6, 7 //top

            /*0, 0, 0, 0, //back
            0, 0, 0, 0, //forward

            0, 0, 0, 0, //left
            0, 0, 0, 0  //right*/
        };
        iarray_size = 8;
        break;
    case (unsigned int)GeometryBuffer::DrawMode::Triangles:
        iarray = new unsigned int[6] { 0, 1, 2, 2, 3, 0 };
        iarray_size = 6;
        break;
    }

    //Create vertex declaration that tells engine in wich sequence data is stored
    MR::VertexDeclarationType vdtypes[2] {
        MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Position, 0),
        MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::TexCoord, (void*)(sizeof(float)* 3))
    };

    MR::VertexDeclaration* vDecl = new MR::VertexDeclaration(&vdtypes[0], 2, MR::VertexDeclaration::DataType::Float);
    MR::IndexDeclaration* iDecl = new MR::IndexDeclaration(MR::IndexDeclaration::DataType::UInt);

    //Create geometry buffer
    MR::GeometryBuffer* gb = new MR::GeometryBuffer(vDecl, iDecl, &garray[0], sizeof(float)*garray_size, &iarray[0], sizeof(unsigned int)*iarray_size, garray_size / per_verts_elements, iarray_size, usage, usage, drawm);
    delete [] iarray;

    return gb;
}

}
