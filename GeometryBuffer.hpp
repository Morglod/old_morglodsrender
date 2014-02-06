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
    VDTE_POSITION = 0, //3 elements
    VDTE_NORMAL = 1, //3 ELEMENTS
    VDTE_TEXTURE_COORD = 2, //2 ELEMENTS
    VDTE_COLOR = 4 //4 ELEMENTS - rgba
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
    inline unsigned short ElementsNum(){
        switch(type){
        case VertexDeclarationTypesEnum::VDTE_POSITION:
            return 3;
            break;
        case VertexDeclarationTypesEnum::VDTE_NORMAL:
            return 3;
            break;
        case VertexDeclarationTypesEnum::VDTE_TEXTURE_COORD:
            return 2;
            break;
        case VertexDeclarationTypesEnum::VDTE_COLOR:
            return 4;
            break;
        }
        return 0;
    }

    const void *pointer;

    VertexDeclarationType() : pointer(nullptr) {}

    //!t - VertexDeclarationTypesEnum
    //!e_num - Num of elements, that describes this data
    //!p - GL pointer in stride
    VertexDeclarationType(VertexDeclarationTypesEnum t, const void *p);
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

    inline bool contains(const VertexDeclarationTypesEnum& type) {
        for(unsigned short i = 0; i < decl_types_num; ++i) {
            if(decl_types[i].type == type) return true;
        }
        return false;
    }

    inline VertexDeclarationType* get(const VertexDeclarationTypesEnum& type) {
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

class InstancedDataType {
public:
    int shader_location = 0;
    GLint size = 0; //number of data elements per instancing element (if vec3 so this value is 3, because of 3 floats)
    GLenum type = GL_FLOAT;
    GLboolean normalized = GL_FALSE;
    GLuint buffer = 0;
    GLsizei stride = 0; //size of one element for instancing (eg sizeof(vec3) )
    GLvoid* pointer = 0;
    unsigned short every_vertexes = 1; //1 means every vertex, 3 means every triangle

    void Bind();
    void Unbind();
    void BufferData(void* data, GLint data_size, GLenum usage);

    InstancedDataType(void* data, GLint data_size, GLenum usage);
    virtual ~InstancedDataType(){}
};

class InstancedDataBuffer {
public:
    InstancedDataType* data_types = nullptr;
    unsigned short data_types_num = 0;
    unsigned short instances_num = 0;

    void Bind();
    void Unbind();
};

class GeometryBuffer : Copyable<GeometryBuffer> {
public:
    /** sender - this GeometryBuffer **/
    MR::Event<const GLenum&> OnDrawModeChanged;
    MR::Event<InstancedDataBuffer*> OnInstDataBufferChanged;

    inline void SetBindInstDataBufferState(const bool & s){ bindInstDataBuffer = s; }
    inline void SetDrawInstDataBufferState(const bool & s){ drawInstDataBuffer = s; }

    void Bind();
    void Unbind();
    void Draw();
    void SetInstData(InstancedDataBuffer* idb);
    void SetDrawMode(const GLenum& dm);

    inline GLenum GetDrawMode(){ return _draw_mode; }
    inline unsigned int GetVertexesNum(){ return _vertexes_num; }
    inline unsigned int GetIndexesNum(){ return _indexes_num; }
    inline GLuint GetVertexBuffer(){ return _vertex_buffer; }
    inline GLuint GetIndexBuffer(){ return _index_buffer; }
    inline GLuint GetVAO(){ return _vao; }
    inline VertexDeclaration* GetVertexDecl(){ return _vdecl; }
    inline IndexDeclaration* GetIndexDecl(){ return _idecl; }
    void GetVData(void* dest_array);
    void GetIData(void* dest_array);
    inline float GetRadius() {return _radius;}

    void CalcRadius(float* data);
    void CalcRadius();

    GeometryBuffer* Copy();
    GeometryBuffer(){}

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
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, GLenum drawm = GL_TRIANGLES, bool calcR = true);

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
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, GLenum usage = GL_STATIC_DRAW, GLenum iusage = GL_STATIC_DRAW, GLenum drawm = GL_TRIANGLES, bool calcR = true);

    //!Don't delete declaration
    virtual ~GeometryBuffer();

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
    unsigned int _vertexes_num = 0, _indexes_num = 0;

    //!OpenGL draw mode
    GLenum _draw_mode = GL_TRIANGLES;

    size_t _data_size = 0, _idata_size = 0;
    GLenum _data_usage = 0, _idata_usage = 0;

    InstancedDataBuffer* instDataBuffer = nullptr;
    bool bindInstDataBuffer = false;
    bool drawInstDataBuffer = false;

    //distance from center point
    float _radius = 0.0f;
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
