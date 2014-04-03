#pragma once

#ifndef _MR_GEOMETRY_BUFFER_H_
#define _MR_GEOMETRY_BUFFER_H_

#include "Events.hpp"
#include "Types.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#   include <glm/gtc/matrix_transform.hpp>
#endif

namespace MR {

class VertexDeclaration;
class VertexDeclarationType;
class IndexDeclaration;
class InstancedDataType;
class InstancedDataBuffer;
class GeometryBuffer;

/**
* Description of stored data in buffer of one type
*/
class VertexDeclarationType {
    friend class VertexDeclaration;
    friend class GeometryBuffer;
public:
    enum class DataType : unsigned char {
        Position = 0, //3 elements
        Normal = 1, //3 ELEMENTS
        TexCoord = 2, //2 ELEMENTS
        Color = 4 //4 ELEMENTS - rgba
    };

    inline unsigned short ElementsNum();
    inline VertexDeclarationType::DataType GetDataType();
    inline const void * GetPointer();

    inline static VertexDeclarationType Position(const void * pointer);
    inline static VertexDeclarationType Normal(const void * pointer);
    inline static VertexDeclarationType TexCoord(const void * pointer);
    inline static VertexDeclarationType Color(const void * pointer);

    //t - VertexDeclarationTypesEnum
    //p - GL pointer in stride
    VertexDeclarationType(const VertexDeclarationType::DataType& t, const void *p);
protected:
    const void * _pointer;
    VertexDeclarationType::DataType _type;
    VertexDeclarationType();
};

/**
* Description of stored data in buffer
*/
class VertexDeclaration {
    friend class VertexDeclarationType;
    friend class GeometryBuffer;
public:
    enum class DataType : unsigned int {
        Float = 0x1406,
        Short = 0x1402,
        Int = 0x1404,
        Double = 0x140A,
        UInt = 0x1405
    };

    inline bool Contains(const VertexDeclarationType::DataType& type);
    inline VertexDeclarationType* Get(const VertexDeclarationType::DataType& type);

    inline VertexDeclaration::DataType GetDataType();
    inline int GetStrideSize();

    //Only if VertexDeclaration created with "VertexDeclaration(const DataType& vertexDataType)" constructor
    VertexDeclarationType* AddType(const VertexDeclarationType::DataType& t, const void *p);
    void ClearTypesList();

    VertexDeclaration(const DataType& vertexDataType);
    VertexDeclaration(VertexDeclarationType* vertexDeclarationType, const unsigned short& vertexDeclarationTypesNum, const DataType& vertexDataType);
    ~VertexDeclaration();
protected:
    VertexDeclarationType* _decl_types = nullptr; //Array of buffer stride elements
    unsigned short _decl_types_num = 0; //Num of elements in array (_decl_types)
    VertexDeclaration::DataType _data_type = VertexDeclaration::DataType::Float;
    int _stride_size = 0;
};

class IndexDeclaration {
    friend class GeometryBuffer;
public:
    enum class DataType : unsigned int {
        Float = 0x1406,
        Short = 0x1402,
        Int = 0x1404,
        Double = 0x140A,
        UInt = 0x1405
    };

    //!dt - OpenGL data type in buffer
    IndexDeclaration(const IndexDeclaration::DataType& dt);
protected:
    IndexDeclaration::DataType _data_type = IndexDeclaration::DataType::UInt;
};

class InstancedDataType {
public:
    enum class DataType : unsigned int {
        Float = 0x1406,
        Short = 0x1402,
        Int = 0x1404,
        Double = 0x140A,
        UInt = 0x1405
    };

    void Bind();
    void Unbind();
    void BufferData(void* data, const int& data_size, const unsigned int& usage);

    InstancedDataType(void* data, const int& data_size, const unsigned int& usage);
    virtual ~InstancedDataType(){}
protected:
    int _shader_location = 0;
    int _size = 0; //number of data elements per instancing element (if vec3 so this value is 3, because of 3 floats)
    InstancedDataType::DataType _type = InstancedDataType::DataType::UInt;
    bool _normalized = false;
    unsigned int _buffer = 0;
    int _stride = 0; //size of one element for instancing (eg sizeof(vec3) )
    void* _pointer = 0;
    unsigned short _every_vertexes = 1; //1 means every vertex, 3 means every triangle
};

class InstancedDataBuffer {
    friend class GeometryBuffer;
public:
    void Bind();
    void Unbind();
protected:
    InstancedDataType* _data_types = nullptr;
    unsigned short _data_types_num = 0;
    unsigned short _instances_num = 0;
};

class GeometryBuffer : Copyable<GeometryBuffer*> {
    friend class InstancedDataBuffer;
    friend class IndexDeclaration;
public:
    enum class DrawMode : unsigned int {
        Points = 0,
        Lines = 1,
        LineLoop = 2,
        LineStrip = 3,
        Triangles = 4,
        TriangleStrip = 5,
        TriangleFan = 6,
        Quads = 7
    };

    enum class Usage : unsigned int {
        Static = 0x88E4,
        Stream = 0x88E0,
        Dynamic = 0x88E8
    };

    /** sender - this GeometryBuffer **/
    MR::Event<const DrawMode&> OnDrawModeChanged;
    MR::Event<InstancedDataBuffer*> OnInstDataBufferChanged;

    inline void SetBindInstDataBufferState(const bool & s){ bindInstDataBuffer = s; }
    inline void SetDrawInstDataBufferState(const bool & s){ drawInstDataBuffer = s; }

    void Bind();
    void Unbind();
    void Draw();
    void SetInstData(InstancedDataBuffer* idb);
    void SetDrawMode(const DrawMode& dm);

    inline DrawMode GetDrawMode(){ return _draw_mode; }
    inline unsigned int GetVertexesNum(){ return _vertexes_num; }
    inline unsigned int GetIndexesNum(){ return _indexes_num; }
    inline unsigned int GetVertexBuffer(){ return _vertex_buffer; }
    inline unsigned int GetIndexBuffer(){ return _index_buffer; }
    inline unsigned int GetVAO(){ return _vao; }
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
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, unsigned int vb, unsigned int ib, unsigned int vnum, unsigned int inum, const DrawMode& drawm = DrawMode::Triangles, bool calcR = true);

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
    GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, const Usage& usage = Usage::Static, const Usage& iusage = Usage::Static, const DrawMode& drawm = DrawMode::Triangles, bool calcR = true);

    //!Don't delete declaration
    virtual ~GeometryBuffer();

    static GeometryBuffer* CreatePlane(const glm::vec3& scale, const glm::vec3 pos, const Usage& usage, const DrawMode& drawm);
    static GeometryBuffer* CreateCube(const glm::vec3& scale, const glm::vec3 pos, const Usage& usage, const DrawMode& drawm);

protected:
    //!Vertex data storage in buffer
    //!Shouldn't be null
    VertexDeclaration* _vdecl = nullptr;

    //!Index data storage in buffer
    IndexDeclaration* _idecl = nullptr;

    //!OpenGL buffers
    //!_vertex_buffer shouldn't be null
    unsigned int _vertex_buffer = 0, _index_buffer = 0, _vao = 0;

    //!Num of vertexes and indexes
    unsigned int _vertexes_num = 0, _indexes_num = 0;

    //!OpenGL draw mode
    DrawMode _draw_mode = DrawMode::Triangles;

    size_t _data_size = 0, _idata_size = 0;
    Usage _data_usage, _idata_usage;

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

MR::VertexDeclarationType::DataType MR::VertexDeclarationType::GetDataType(){
    return _type;
}

const void * MR::VertexDeclarationType::GetPointer(){
    return _pointer;
}

MR::VertexDeclarationType MR::VertexDeclarationType::Position(const void * pointer){
    return MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Position, pointer);
}

MR::VertexDeclarationType MR::VertexDeclarationType::Normal(const void * pointer){
    return MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Normal, pointer);
}

MR::VertexDeclarationType MR::VertexDeclarationType::TexCoord(const void * pointer){
    return MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::TexCoord, pointer);
}

MR::VertexDeclarationType MR::VertexDeclarationType::Color(const void * pointer){
    return MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Color, pointer);
}

unsigned short MR::VertexDeclarationType::ElementsNum(){
    switch(_type){
    case VertexDeclarationType::DataType::Position:
    case VertexDeclarationType::DataType::Normal:
        return 3;
        break;
    case VertexDeclarationType::DataType::TexCoord:
        return 2;
        break;
    case VertexDeclarationType::DataType::Color:
        return 4;
        break;
    }
    return 0;
}

bool MR::VertexDeclaration::Contains(const VertexDeclarationType::DataType& type) {
    for(unsigned short i = 0; i < _decl_types_num; ++i) {
        if(_decl_types[i]._type == type) return true;
    }
    return false;
}

MR::VertexDeclarationType* MR::VertexDeclaration::Get(const VertexDeclarationType::DataType& type) {
    for(unsigned short i = 0; i < _decl_types_num; ++i) {
        if(_decl_types[i]._type == type) return &_decl_types[i];
    }
    return nullptr;
}

MR::VertexDeclaration::DataType MR::VertexDeclaration::GetDataType(){
    return _data_type;
}

int MR::VertexDeclaration::GetStrideSize(){
    return _stride_size;
}

#endif // _MR_GEOMETRY_BUFFER_H
