#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "Log.hpp"
#include "Transformation.hpp"

namespace MR
{
VertexDeclarationType::VertexDeclarationType(VertexDeclarationTypesEnum t, unsigned short e_num, const GLvoid *p) : pointer(p)
{
    this->type = t;
    this->elements_num = e_num;
}

VertexDeclaration::VertexDeclaration(VertexDeclarationType* vdt, unsigned short dn, GLenum dt)
{
    this->decl_types = vdt;
    this->decl_types_num = dn;
    this->data_type = dt;

    switch(dt)
    {
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
    for(unsigned short i = 0; i < dn; ++i)
    {
        e_num += decl_types[i].elements_num;
    }
    this->stride_size *= e_num;
}

VertexDeclaration::~VertexDeclaration()
{
    delete [] this->decl_types;
    this->decl_types = nullptr;
}

IndexDeclaration::IndexDeclaration(GLenum dt) : data_type(dt) {}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, GLenum drawm)
{
    this->_draw_mode = drawm;
    this->_vdecl = vd;
    this->_idecl = id;
    this->_vertex_buffer = vb;
    this->_index_buffer = ib;
    this->_vertexes_num = vnum;
    this->_indexes_num = inum;
}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, GLenum usage, GLenum iusage, GLenum drawm)
 : _vdecl(vd), _idecl(id), _vertexes_num(vnum), _indexes_num(inum), _draw_mode(drawm)
{
    //Create vertex array
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    //Create data buffer
    glGenBuffers(1, &_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);

    //Point attributes

    VertexDeclarationType* v_pos = _vdecl->get(VertexDeclarationTypesEnum::VDTE_POSITION);
    if(v_pos != nullptr){
        glVertexAttribPointer(SHADER_VERTEX_POSITION_ATTRIB_LOCATION, v_pos->elements_num, _vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_pos->pointer);
        glEnableVertexAttribArray(SHADER_VERTEX_POSITION_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_texcoord = _vdecl->get(VertexDeclarationTypesEnum::VDTE_TEXTURE_COORD);
    if(v_texcoord != nullptr){
        glVertexAttribPointer(SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, v_texcoord->elements_num, _vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_texcoord->pointer);
        glEnableVertexAttribArray(SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_norm = _vdecl->get(VertexDeclarationTypesEnum::VDTE_NORMAL);
    if(v_norm != nullptr){
        glVertexAttribPointer(SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, v_norm->elements_num, _vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_norm->pointer);
        glEnableVertexAttribArray(SHADER_VERTEX_NORMAL_ATTRIB_LOCATION);
    }

    VertexDeclarationType* v_color = _vdecl->get(VertexDeclarationTypesEnum::VDTE_COLOR);
    if(v_color != nullptr){
        glVertexAttribPointer(SHADER_VERTEX_COLOR_ATTRIB_LOCATION, v_color->elements_num, _vdecl->data_type, GL_FALSE, _vdecl->stride_size, v_color->pointer);
        glEnableVertexAttribArray(SHADER_VERTEX_COLOR_ATTRIB_LOCATION);
    }

    //Unbind vertex array
    glBindVertexArray(0);

    //Create index buffer
    if(_idecl != nullptr){
        glGenBuffers(1, &this->_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idata_size, idata, iusage);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
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
