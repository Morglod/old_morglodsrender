#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "Log.hpp"
#include "Transformation.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

VertexDeclarationType::VertexDeclarationType(VertexDeclarationTypesEnum t, const GLvoid *p) : type(t), pointer(p) {
}

VertexDeclaration::VertexDeclaration(VertexDeclarationType* vdt, const unsigned short& dn, const DataType& dt) {
    this->decl_types = vdt;
    this->decl_types_num = dn;
    this->data_type = dt;

    switch( (int)dt ) {
    case GL_INT:
        this->stride_size = sizeof(int);
        break;
    case GL_SHORT:
        this->stride_size = sizeof(short);
        break;
    case GL_FLOAT:
        this->stride_size = sizeof(float);
        break;
    case GL_DOUBLE:
        this->stride_size = sizeof(double);
        break;
    default:
        this->stride_size = sizeof(float);
        break;
    }

    unsigned int e_num = 0;
    for(unsigned short i = 0; i < dn; ++i) {
        e_num += decl_types[i].ElementsNum();
    }
    this->stride_size *= e_num;
}

VertexDeclaration::~VertexDeclaration() {
    delete [] this->decl_types;
    this->decl_types = nullptr;
}

IndexDeclaration::IndexDeclaration(const DataType& dt) : data_type(dt) {}

void InstancedDataType::Bind() {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glVertexAttribPointer(shader_location, size, (unsigned int)type, normalized, stride, pointer);
    glEnableVertexAttribArray(shader_location);
    glVertexAttribDivisor(shader_location, every_vertexes);
}

void InstancedDataType::Unbind()  {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedDataType::BufferData(void* data, GLint data_size, GLenum usage) {
    size = data_size;
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

InstancedDataType::InstancedDataType(void* data, GLint data_size, GLenum usage) : size(data_size) {
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void InstancedDataBuffer::Bind() {
    for(unsigned short it = 0; it < data_types_num; ++it) {
        data_types[it].Bind();
    }
}

void InstancedDataBuffer::Unbind() {
    for(unsigned short it = 0; it < data_types_num; ++it) {
        data_types[it].Unbind();
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
        if(_idecl == nullptr) glDrawArraysInstanced((unsigned int)_draw_mode, 0, _vertexes_num, instDataBuffer->instances_num);
        else glDrawElementsInstanced((unsigned int)_draw_mode, _indexes_num, (unsigned int)_idecl->data_type, (void*)0, instDataBuffer->instances_num);
    } else {
        if(_idecl == nullptr) glDrawArrays((unsigned int)_draw_mode, 0, _vertexes_num);
        else glDrawElements((unsigned int)_draw_mode, _indexes_num, (unsigned int)_idecl->data_type, (void*)0);
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
    for(unsigned short it = 0; it < _vdecl->decl_types_num; ++it) {
        if(_vdecl->decl_types[it].type == VertexDeclarationTypesEnum::VDTE_POSITION) {
            break;
        }
        pos_offset += _vdecl->decl_types[it].ElementsNum();
    }
    for(unsigned short it = 0; it < _vdecl->decl_types_num; ++it) {
        stride += _vdecl->decl_types[it].ElementsNum();
    }

#ifdef MR_USE_OPENMP
    #pragma omp parallel for
    for(unsigned int it = pos_offset; it < (_vertexes_num*stride); it += stride ) {
        point[0] = std::max( point[0], std::abs(data[it  ]) );
        point[1] = std::max( point[1], std::abs(data[it+1]) );
        point[2] = std::max( point[2], std::abs(data[it+2]) );
    }
#else
    for(unsigned int it = pos_offset; it < (_vertexes_num*stride); it += stride ) {
        point[0] = std::max( point[0], std::abs(data[it  ]) );
        point[1] = std::max( point[1], std::abs(data[it+1]) );
        point[2] = std::max( point[2], std::abs(data[it+2]) );
    }
#endif // MR_USE_OPENMP

    _radius = glm::length( glm::vec3(point[0], point[1], point[2]) );
}

void GeometryBuffer::CalcRadius() {
    unsigned char* data = new unsigned char[_data_size];
    GetVData(data);
    CalcRadius((float*)data);
    delete [] data;
}

void GeometryBuffer::GetVData(void* da) {
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glGetBufferSubData(	GL_ARRAY_BUFFER, 0, _data_size, da);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GeometryBuffer::GetIData(void* da) {
    glBindBuffer(GL_ARRAY_BUFFER, _index_buffer);
    glGetBufferSubData(	GL_ARRAY_BUFFER, 0, _data_size, da);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

GeometryBuffer* GeometryBuffer::Copy() {
    VertexDeclarationType vdtc[this->_vdecl->decl_types_num];
    for(unsigned short it = 0; it < this->_vdecl->decl_types_num; ++it) {
        vdtc[it] = VertexDeclarationType(this->_vdecl->decl_types[it].type, this->_vdecl->decl_types[it].pointer);
    }
    VertexDeclaration* vdc = new VertexDeclaration(&vdtc[0], this->_vdecl->decl_types_num, this->_vdecl->data_type);

    IndexDeclaration* idc = new IndexDeclaration(this->_idecl->data_type);

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
    glBindVertexArray(_vao);

    //Create data buffer
    glGenBuffers(1, &_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, (unsigned int)usage);

    //Point attributes

    VertexDeclarationType* v_pos = _vdecl->get(VertexDeclarationTypesEnum::VDTE_POSITION);
    if(v_pos != nullptr) {
        glVertexAttribPointer(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, v_pos->ElementsNum(), (unsigned int)_vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_pos->pointer);
        glEnableVertexAttribArray(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_texcoord = _vdecl->get(VertexDeclarationTypesEnum::VDTE_TEXTURE_COORD);
    if(v_texcoord != nullptr) {
        glVertexAttribPointer(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, v_texcoord->ElementsNum(), (unsigned int)_vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_texcoord->pointer);
        glEnableVertexAttribArray(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_norm = _vdecl->get(VertexDeclarationTypesEnum::VDTE_NORMAL);
    if(v_norm != nullptr) {
        glVertexAttribPointer(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, v_norm->ElementsNum(), (unsigned int)_vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_norm->pointer);
        glEnableVertexAttribArray(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_color = _vdecl->get(VertexDeclarationTypesEnum::VDTE_COLOR);
    if(v_color != nullptr) {
        glVertexAttribPointer(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION, v_color->ElementsNum(), (unsigned int)_vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_color->pointer);
        glEnableVertexAttribArray(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION);
    }

    //Unbind vertex array
    glBindVertexArray(0);

    //Create index buffer
    if(_idecl != nullptr) {
        glGenBuffers(1, &this->_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idata_size, idata, (unsigned int)iusage);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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

}
