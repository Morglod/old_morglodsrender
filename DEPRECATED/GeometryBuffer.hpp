#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

/*
//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

//#include "MorglodsRender.h"

#include <iostream>*/
#include "pre.hpp"
#include "Shader.hpp"

namespace MR
{

//Type of stored data
enum VertexDeclarationTypesEnum
{
    VDTE_POSITION = 0,
    VDTE_NORMAL = 1, //!ONLY 3 ELEMENTS
    VDTE_TEXTURE_COORD = 2,
    VDTE_COLOR = 4
};

//Description of stored data in buffer of one type
struct VertexDeclarationType
{
public:
    //Type of stored data
    VertexDeclarationTypesEnum type = VertexDeclarationTypesEnum::VDTE_POSITION;

    //Num of elements, that describes this data
    //!If VDTE_NORMAL, elements_num = 3
    unsigned short elements_num;

    const GLvoid *pointer;

    VertexDeclarationType(){}

    //t - VertexDeclarationTypesEnum
    //e_num - Num of elements, that describes this data
    //p - GL pointer in stride
    VertexDeclarationType(VertexDeclarationTypesEnum t, unsigned short e_num, const GLvoid *p);
};

//Description of stored data in buffer
struct VertexDeclaration
{
public:
    //Array of buffer stride elements
    VertexDeclarationType* _decl_types = nullptr;

    //Num of elements in array (_decl_types)
    unsigned short _decl_types_num = 0;

    inline bool contains(VertexDeclarationTypesEnum type){
        for(unsigned short i = 0; i < _decl_types_num; ++i){
            if(_decl_types[i].type == type) return true;
        }
        return false;
    }

    inline VertexDeclarationType* get(VertexDeclarationTypesEnum type){
        for(unsigned short i = 0; i < _decl_types_num; ++i){
            if(_decl_types[i].type == type) return &_decl_types[i];
        }
        return nullptr;
    }

    //Data type in buffer
    GLenum _data_type = GL_FLOAT;

    //Size of stride for OpenGL
    GLsizei stride_size = 0;

    //vdt - VertexDeclarationType array
    //dn - Num of elements in vdt array
    //dt - Data type
    VertexDeclaration(VertexDeclarationType* vdt, unsigned short dn, GLenum dt);

    ~VertexDeclaration();
};

struct IndexDeclaration
{
public:
    //OpenGL type of data in buffer
    GLenum _data_type = GL_UNSIGNED_INT;

    //dt - OpenGL data type in buffer
    IndexDeclaration(GLenum dt);
};

class GeometryBuffer
{
protected:
    //Vertex data storage in buffer
    //Shouldn't be null
    VertexDeclaration* _decl = nullptr;

    //Index data storage in buffer
    IndexDeclaration* _idecl = nullptr;

    //OpenGL buffers
    //_vertex_buffer shouldn't be null
    GLuint _vertex_buffer = 0, _index_buffer = 0;

    //Num of vertexes and indexes
    unsigned int _vertexes_num = 3, _indexes_num = 0;

    //OpenGL draw mode
    GLenum draw_mode = GL_TRIANGLES;

public:
    inline void Bind(){
        std::cout << "BIND" << '\n';

        if(this->_vertex_buffer == 0) return;
        if(this->_decl == nullptr) return;

        std::cout << "decl types num " << this->_decl->_decl_types_num;
        for(unsigned short i = 0; i < this->_decl->_decl_types_num; ++i)
        {
            std::cout << "decl type" << '\n';
            switch(this->_decl->_decl_types[i].type)
            {
            case VDTE_POSITION:
                glEnableClientState(GL_VERTEX_ARRAY);
                break;
            case VDTE_NORMAL:
                glEnableClientState(GL_NORMAL_ARRAY);
                break;
            case VDTE_TEXTURE_COORD:
                glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                break;
            case VDTE_COLOR:
                glEnableClientState(GL_COLOR_ARRAY);
                break;
            }
        }

        std::cout << "BIND1" << '\n';

        glBindBuffer(GL_ARRAY_BUFFER, this->_vertex_buffer);

        //BIND
        VertexDeclarationType* v_pos = _decl->get(VertexDeclarationTypesEnum::VDTE_POSITION);
        if(v_pos != nullptr){
            glVertexAttribPointer(SHADER_VERTEX_POSITION_ATTRIB_LOCATION, v_pos->elements_num, _decl->_data_type, GL_FALSE, _decl->stride_size, v_pos->pointer);
            glEnableVertexAttribArray(SHADER_VERTEX_POSITION_ATTRIB_LOCATION);
        }

        VertexDeclarationType* v_texcoord = _decl->get(VertexDeclarationTypesEnum::VDTE_TEXTURE_COORD);
        if(v_texcoord != nullptr){
            glVertexAttribPointer(SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, v_texcoord->elements_num, _decl->_data_type, GL_FALSE, _decl->stride_size, v_texcoord->pointer);
            glEnableVertexAttribArray(SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION);
        }

        VertexDeclarationType* v_norm = _decl->get(VertexDeclarationTypesEnum::VDTE_NORMAL);
        if(v_norm != nullptr){
            glVertexAttribPointer(SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, v_norm->elements_num, _decl->_data_type, GL_FALSE, _decl->stride_size, v_norm->pointer);
            glEnableVertexAttribArray(SHADER_VERTEX_NORMAL_ATTRIB_LOCATION);
        }

        VertexDeclarationType* v_color = _decl->get(VertexDeclarationTypesEnum::VDTE_COLOR);
        if(v_color != nullptr){
            glVertexAttribPointer(SHADER_VERTEX_COLOR_ATTRIB_LOCATION, v_color->elements_num, _decl->_data_type, GL_FALSE, _decl->stride_size, v_color->pointer);
            glEnableVertexAttribArray(SHADER_VERTEX_COLOR_ATTRIB_LOCATION);
        }
        //-------

        std::cout << "BIND2" << '\n';

        //! todo: TRY TO ADD enableClientState to this cycle
        /*for(unsigned short i = 0; i < this->_decl->_decl_types_num; ++i)
        {
            switch(this->_decl->_decl_types[i].type)
            {
            case VDTE_POSITION:
                glVertexPointer(this->_decl->_decl_types[i].elements_num, this->_decl->_data_type, this->_decl->stride_size, this->_decl->_decl_types[i].pointer);
                break;
            case VDTE_NORMAL:
                glNormalPointer(this->_decl->_data_type, this->_decl->stride_size, this->_decl->_decl_types[i].pointer);
                break;
            case VDTE_TEXTURE_COORD:
                glTexCoordPointer(this->_decl->_decl_types[i].elements_num, this->_decl->_data_type, this->_decl->stride_size, this->_decl->_decl_types[i].pointer);
                break;
            case VDTE_COLOR:
                glColorPointer(this->_decl->_decl_types[i].elements_num, this->_decl->_data_type, this->_decl->stride_size, this->_decl->_decl_types[i].pointer);
                break;
            }
        }*/
        if(_index_buffer != 0)
        {
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer );
            glEnableClientState( GL_ELEMENT_ARRAY_BUFFER );
        }
        std::cout << "BIND OK" << '\n';
    }

    inline void Unbind(){
        if(this->_vertex_buffer == 0) return;
        if(this->_decl == nullptr) return;

        if(_index_buffer != 0){
            glDisableClientState( GL_ELEMENT_ARRAY_BUFFER );
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        for(unsigned short i = this->_decl->_decl_types_num-1; i > 0 ; --i)
        {
            switch(this->_decl->_decl_types[i].type)
            {
            case VDTE_POSITION:
                glDisableClientState(GL_VERTEX_ARRAY);
                break;
            case VDTE_NORMAL:
                glDisableClientState(GL_NORMAL_ARRAY);
                break;
            case VDTE_TEXTURE_COORD:
                glDisableClientState(GL_TEXTURE_COORD_ARRAY);
                break;
            case VDTE_COLOR:
                glDisableClientState(GL_COLOR_ARRAY);
                break;
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    inline void Draw()
    {
        if(this->_vertex_buffer == 0) return;
        if(this->_decl == nullptr) return;

        if(_index_buffer != 0)
        {
            glEnableClientState( GL_ELEMENT_ARRAY_BUFFER );

            glDrawElements(
                this->draw_mode, //mode
                _indexes_num, //count
                _idecl->_data_type, //type
                0 //element array buffer offset
            );

            glDisableClientState( GL_ELEMENT_ARRAY_BUFFER );
        }
        else{
            //std::cout << "\nDRAW ARRAYS " << this->draw_mode << ", 0, " << this->_vertexes_num << '\n';
            glDrawArrays(this->draw_mode, 0, this->_vertexes_num);
        }
    }

    GeometryBuffer(){}

    //vd - VertexDeclaration pointer
    //id - IndexDeclaration pointer
    //vb - OpenGL vertex buffer
    //ib - OpenGL index buffer
    //dt - OpenGL data type in buffer
    //vnum - Number of vertices
    //inum - Number of indexes
    //drawm - OpenGL draw mode GL_POINTERS GL_LINES GL_TRIANGLES GL_QUADS
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, GLenum drawm = GL_TRIANGLES);

    //vd - VertexDeclaration pointer
    //id - IndexDeclaration pointer
    //data - Pointer to data
    //data_size - Size of passed data
    //idata - Pointer to indexdata
    //idata_size - Size of passed index data
    //vnum - Number of vertices
    //inum - Number of indexes
    //usage - OpenGL buffer usage (for example GL_STATIC_DRAW)
    //iusage - OpenGL index buffer usage (for example GL_STATIC_DRAW)
    //drawm - OpenGL draw mode GL_POINTERS GL_LINES GL_TRIANGLES GL_QUADS
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, GLenum usage = GL_STATIC_DRAW, GLenum iusage = GL_STATIC_DRAW, GLenum drawm = GL_TRIANGLES);

    //Don not deletes declaration
    ~GeometryBuffer();
};
}

#endif // _MR_GEOMETRY_BUFFER_H

//-------------------
//Handles float3 info
//-------------------
/*struct Float3{
protected:
    float* farray;
public:
    //GET
    float X(){ return farray[0]; }
    float Y(){ return farray[1]; }
    float Z(){ return farray[2]; }
    float* XY(){ return new float[2]{ farray[0], farray[1] }; }
    float* YZ(){ return new float[2]{ farray[1], farray[2] }; }
    float* XZ(){ return new float[2]{ farray[0], farray[2] }; }
    float* XYZ(){ return new float[3]{ farray[0], farray[1], farray[2] }; }

    //SET
    void X(float x){ farray[0] = x; }
    void Y(float y){ farray[1] = y; }
    void Z(float z){ farray[2] = z; }
    void XY(float x, float y){ farray[0] = x; farray[1] = y; }
    void YZ(float y, float z){ farray[1] = y; farray[2] = z; }
    void XZ(float x, float z){ farray[0] = x; farray[2] = z; }
    void XYZ(float x, float y, float z){ farray[0] = x; farray[1] = y; farray[2] = z; }

    Float3(){ farray = new float[3]; }
    Float3(float x, float y, float z){ farray = new float[3]{x, y, z}; }
} typedef Float3;*/
/*
    //-------------------------
    //Interface for geom buffer
    //-------------------------
    class IGeomBuffer{
    public:
        //---------------------
        //Returns OpenGL Buffer
        virtual GLuint Get() = 0;

        //-----------------------------------------------------------
        //Binds OpenGL buffer and enables OpenGL client states if any
        virtual void Bind() = 0;

        //-------------------------------------------------------------
        //Unbinds OpenGL buffer and disables OpenGL client state if any
        virtual void Unbind() = 0;

        //----------------------------------------
        //Returns num of elements in OpenGL buffer
        virtual unsigned int GetElementsNum() = 0;

        virtual ~IGeomBuffer(){}
    };

    //------------------------------------
    //Handles custom data, used with verts
    //------------------------------------
    class DataBuffer : public IGeomBuffer{
    protected:
        unsigned int elements_num = 0; //num of elements in buffer (not 3d coords)
        int elements_size = 0; //total size of buffer
        GLuint gl_buffer = 0; //gl buffer
        GLenum gl_buffer_type; //GL_VERTEX_ARRAY/GL_NORMAL_ARRAY/GL_TEXTURE_COORD_ARRAY/GL_COLOR_ARRAY
        GLenum gl_buffer_data_type; //GL_FLOAT etc
    public:
		//----------------------------------------------------
		//elements - pointer to elements array for data buffer
		//element_size - size of type of elements in elements array
		//num - num of elements in elements array
		//mode - eg GL_STATIC_DRAW
		//type - one of this flags GL_VERTEX_ARRAY/GL_NORMAL_ARRAY/GL_TEXTURE_COORD_ARRAY/GL_COLOR_ARRAY
		//data_type - GL_FLOAT etc
		//!!WARNING!!
		//For different buffer types, you should pass different elements
		//For GL_VERTEX_ARRAY (x,y,z)
		//For GL_NORMAL_ARRAY (x,y,z)
		//For GL_TEXTURE_COORD_ARRAY (x,y)
		//For GL_COLOR_ARRAY (r,g,b)
        //void Set(void* elements, int element_size, unsigned int num, GLenum mode, GLenum type, GLenum data_type);

        //--------------------------------------------------------------------------------
        //Binds OpenGL buffer (gl_BUFFER) and enables OpenGL client state (gl_BUFFER_TYPE)
        void Bind();

        //---------------------------------------------
        //Disables OpenGL client state (gl_BUFFER_TYPE)
        inline void Unbind(){glDisableClientState( this->gl_BUFFER_TYPE );}

        //-----------------------------
        //Returns gl buffer (gl_BUFFER)
        inline GLuint Get(){return this->gl_BUFFER;}

        //--------------------------------------
        //Returns num of elements (elements_num)
        inline unsigned int GetElementsNum(){return this->elements_num;}

        //---------------------------------------
        //Returns gl buffer type (gl_BUFFER_TYPE)
        inline GLenum GetBufferType(){return this->gl_BUFFER_TYPE;}

        //----------------------------------------------------
		//elements - pointer to elements array for data buffer
		//element_size - size of type of elements in elements array
		//num - num of elements in elements array
		//mode - eg GL_STATIC_DRAW
		//type - one of this flags GL_VERTEX_ARRAY/GL_NORMAL_ARRAY/GL_TEXTURE_COORD_ARRAY/GL_COLOR_ARRAY
		//data_type - GL_FLOAT etc
		//!!WARNING!!
		//For different buffer types, you should pass different elements
		//For GL_VERTEX_ARRAY (x,y,z)
		//For GL_NORMAL_ARRAY (x,y,z)
		//For GL_TEXTURE_COORD_ARRAY (x,y)
		//For GL_COLOR_ARRAY (r,g,b)
		DataBuffer(void* elements, int element_size, unsigned int num, GLenum mode, GLenum type, GLenum data_type);

		//-------------------------
		//Deletes handled gl buffer
		~DataBuffer();
    };

    //--------------------------
    //Handles vertex information
    //--------------------------
    class VertexBuffer : public IGeomBuffer{
    protected:
        unsigned int elements_num = 0; //num of floats (not 3d coords)
        int elements_size = 0; //total size of buffer
        GLuint gl_BUFFER = 0; //gl buffer
    public:
        void* elements_copy = NULL; //data may be backupped

        DataBuffer** dataBuffers = NULL; //array of pointers to DataBuffers; can contain one of any type of array buffers (GL_VERTEX_ARRAY/GL_NORMAL_ARRAY/GL_TEXTURE_COORD_ARRAY/GL_COLOR_ARRAY); used while drawing (Draw method)
        unsigned int dataBuffers_num = 0;

        //-------------------------------------------------------------------
        //Binds handled vertex buffer (gl_BUFFER) and enables gl client state
        virtual void Bind();

        //----------------
        //Disable gl state
        virtual inline void Unbind(){glDisableClientState( GL_VERTEX_ARRAY );}

        //-----------------------------
        //Returns gl buffer (gl_BUFFER)
        virtual inline GLuint Get(){return this->gl_BUFFER;}

        //------------------------------------------------
        //Returns num of elements in buffer (elements_num)
        virtual inline unsigned int GetElementsNum(){return this->elements_num;}

        //----------------------------------------------------
		//elements - pointer to elements array for data buffer
		//num - num of elements in elements array
		//mode - eg GL_STATIC_DRAW
		//!!WARNING!!
		//Elements should contain only (x,y,z) in float format
        VertexBuffer(void* elements, unsigned int num, GLenum mode); //mode -> GL_STATIC_DRAW

        //------------------------------------------------------
		//Deletes handled gl buffer and sets to zero dataBuffers
        virtual ~VertexBuffer();
    };

    //-------------------------
    //Handles index information
    //-------------------------
    class IndexBuffer : public IGeomBuffer{
    protected:
        unsigned int elements_num = 0; //num of uints
        GLuint gl_BUFFER = 0; //gl buffer
    public:
        unsigned int * elements_copy = NULL; //data may be backupped

        //-------------------------------------------------------------------
        //Binds handled index buffer (gl_BUFFER) and enables gl client state
        void Bind();

        //----------------
        //Disable gl state
        inline void Unbind(){glDisableClientState( GL_ELEMENT_ARRAY_BUFFER );}

        //-----------------------------
        //Returns gl buffer (gl_BUFFER)
        inline GLuint Get(){return this->gl_BUFFER;}

        //------------------------------------------------
        //Returns num of elements in buffer (elements_num)
        inline unsigned int GetElementsNum(){return this->elements_num;}

        //----------------------------------------------------
		//elements - pointer to elements array for data buffer
		//element_size - size of type of elements in elements array
		//num - num of elements in elements array
		//mode - eg GL_STATIC_DRAW
		//!!WARNING!!
		//Elements should contain only (x,y,z) in float format
        IndexBuffer(unsigned int * elements, unsigned int num, GLenum mode); //mode -> GL_STATIC_DRAW

        //------------------------------------------------------
		//Deletes handled gl buffer
        ~IndexBuffer();
    };

    class FullGeometryBuffer{
    protected:
        GLuint gl_BUFFER = 0; //gl buffer
        GLuint gl_va = 0; //gl vertex array
    public:
        unsigned int gl_array_start_index = 0, gl_array_size = 0;

        //num - num of vertices (xyz - pos, xyz - normal, xy - tex coord)
        FullGeometryBuffer(void* elements, unsigned int num, GLenum mode);
    };

    //-------------------------------------------------------------------------
    //Handles one Vertex & Index buffers and drawing information (drawing mode)
    //-------------------------------------------------------------------------
    class GeometryBuffer : public virtual Node {
    public:
        GLenum gl_DRAW_MODE;
        VertexBuffer* vertexBuffer = NULL;
        IndexBuffer* indexBuffer = NULL;
        FullGeometryBuffer* fullGeometryBuffer = nullptr;

        //-----------------------------------------------------
        //Draws buffers; if indexBuffer is NULL it wont be used
        void Draw();

        inline std::type_index GetType(){
            return typeid(MR::GeometryBuffer);
        }

        GeometryBuffer(){};
        //---------------------------------------
        //vb - VertexBuffer; Should be not NULL!!
        //ib - IndexBuffer; Can be NULL
        //dm - draw mode (GL_POINTS / GL_TRIANGLES etc)
        GeometryBuffer(VertexBuffer* vb, IndexBuffer* ib, GLenum dm);

        //-----------------------------------------------------
        //Sets to zero vertexBuffer, indexBuffer and parentNode
        ~GeometryBuffer();
    };*/
