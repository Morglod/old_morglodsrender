#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "pre.hpp"
#include "Events.hpp"

namespace MR {

/**
* Type of stored data
*/
enum VertexDeclarationTypesEnum {
    VDTE_POSITION = 0,
    VDTE_NORMAL = 1, //!ONLY 3 ELEMENTS
    VDTE_TEXTURE_COORD = 2, //!ONLY 2 ELEMENTS
    VDTE_COLOR = 4
};

/**
* Description of stored data in buffer of one type
*/
struct VertexDeclarationType {
public:
    //!Type of stored data
    VertexDeclarationTypesEnum type = VertexDeclarationTypesEnum::VDTE_POSITION;

    //!Num of elements, that describes this data
    //!If VDTE_NORMAL, elements_num = 3
    //!If VDTE_TEXTURE_COORD, elements_num = 2
    unsigned short elements_num;

    const GLvoid *pointer;

    VertexDeclarationType() {}

    //!t - VertexDeclarationTypesEnum
    //!e_num - Num of elements, that describes this data
    //!p - GL pointer in stride
    VertexDeclarationType(VertexDeclarationTypesEnum t, unsigned short e_num, const GLvoid *p);
};

/**
* Description of stored data in buffer
*/
struct VertexDeclaration {
public:
    //!Array of buffer stride elements
    VertexDeclarationType* decl_types = nullptr;

    //!Num of elements in array (_decl_types)
    unsigned short decl_types_num = 0;

    inline bool contains(VertexDeclarationTypesEnum type) {
        for(unsigned short i = 0; i < decl_types_num; ++i) {
            if(decl_types[i].type == type) return true;
        }
        return false;
    }

    inline VertexDeclarationType* get(VertexDeclarationTypesEnum type) {
        for(unsigned short i = 0; i < decl_types_num; ++i) {
            if(decl_types[i].type == type) return &decl_types[i];
        }
        return nullptr;
    }

    //1Data type in buffer
    GLenum data_type = GL_FLOAT;

    //!Size of stride for OpenGL
    GLsizei stride_size = 0;

    //!vdt - VertexDeclarationType array
    //!dn - Num of elements in vdt array
    //!dt - Data type
    VertexDeclaration(VertexDeclarationType* vdt, unsigned short dn, GLenum dt);

    ~VertexDeclaration();
};

struct IndexDeclaration {
public:
    //!OpenGL type of data in buffer
    GLenum data_type = GL_UNSIGNED_INT;

    //!dt - OpenGL data type in buffer
    IndexDeclaration(GLenum dt);
};

class GeometryBuffer {
protected:
    //!Vertex data storage in buffer
    //!Shouldn't be null
    VertexDeclaration* _vdecl = nullptr;

    //!Index data storage in buffer
    IndexDeclaration* _idecl = nullptr;

    //!OpenGL buffers
    //!_vertex_buffer shouldn't be null
    GLuint _vertex_buffer = 0, _index_buffer = 0, _vao = 0;

    //!Num of vertexes and indexes
    unsigned int _vertexes_num = 3, _indexes_num = 0;

    //!OpenGL draw mode
    GLenum _draw_mode = GL_TRIANGLES;

public:

    //!sender - geometry buffer
    //!arg1 - old draw mode
    //!arg2 - new draw mode
    MR::Event<GLenum, GLenum> OnDrawModeChanged;

    inline void Bind() {
        glBindVertexArray(_vao);
        if(_idecl != nullptr){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _index_buffer);
        }
    }

    inline void Unbind() {
        if(_idecl != nullptr){
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }
        glBindVertexArray(0);
    }

    inline void Draw() {
        if(_idecl == nullptr) glDrawArrays(_draw_mode, 0, _vertexes_num);
        else glDrawElements(_draw_mode, _indexes_num, _idecl->data_type, (void*)0);
    }

    inline void SetDrawMode(GLenum dm){
        OnDrawModeChanged(this, _draw_mode, dm);
        _draw_mode = dm;
    }

    inline GLenum GetDrawMode(){
        return _draw_mode;
    }

    inline unsigned int GetVertexesNum(){
        return _vertexes_num;
    }

    inline unsigned int GetIndexesNum(){
        return _indexes_num;
    }

    inline GLuint GetVertexBuffer(){
        return _vertex_buffer;
    }

    inline GLuint GetIndexBuffer(){
        return _index_buffer;
    }

    inline GLuint GetVAO(){
        return _vao;
    }

    inline VertexDeclaration* GetVertexDecl(){
        return _vdecl;
    }

    inline IndexDeclaration* GetIndexDecl(){
        return _idecl;
    }

    GeometryBuffer() {}

    /**
    * vd - VertexDeclaration pointer
    * id - IndexDeclaration pointer
    * vb - OpenGL vertex buffer
    * ib - OpenGL index buffer
    * dt - OpenGL data type in buffer
    * vnum - Number of vertices
    * inum - Number of indexes
    * drawm - OpenGL draw mode GL_POINTERS GL_LINES GL_TRIANGLES GL_QUADS
    */
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, GLenum drawm = GL_TRIANGLES);

    /** vd - VertexDeclaration pointer
    * id - IndexDeclaration pointer
    * data - Pointer to data
    * data_size - Size of passed data
    * idata - Pointer to indexdata
    * idata_size - Size of passed index data
    * vnum - Number of vertices
    * inum - Number of indexes
    * usage - OpenGL buffer usage (for example GL_STATIC_DRAW)
    * iusage - OpenGL index buffer usage (for example GL_STATIC_DRAW)
    * drawm - OpenGL draw mode GL_POINTERS GL_LINES GL_TRIANGLES GL_QUADS
    */
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, GLenum usage = GL_STATIC_DRAW, GLenum iusage = GL_STATIC_DRAW, GLenum drawm = GL_TRIANGLES);

    //!Don't delete declaration
    ~GeometryBuffer();
};

/** Imports geometry from file
 *  file - Path to mom file
 *  buffers - link to array of pointers, where will be saved imported geometry
 *  num - link to var where will be saved geometry buffers num
 *  indexes - import indexes from file? true by default
 *  log - log debug messages? false by default
 */
bool ImportMoGeom(std::string file, MR::GeometryBuffer**& buffers, unsigned int & num, bool indexes = true, bool log = false);
}

#endif // _MR_GEOMETRY_BUFFER_H
