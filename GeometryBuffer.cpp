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

GeometryBuffer::~GeometryBuffer(){
    if(this->_vertex_buffer != 0) glDeleteBuffers(1, &this->_vertex_buffer);
    if(this->_index_buffer != 0) glDeleteBuffers(1, &this->_index_buffer);
    if(this->_vao != 0) glDeleteVertexArrays(1, &this->_vao);
}

bool ImportMoGeom(std::string file, MR::GeometryBuffer**& buffers, unsigned int & num, bool bindexes, bool log){
    if(log) MR::Log::LogString("Importing geometry from ("+file+")", MR_LOG_LEVEL_INFO);
    std::ifstream ffile(file, std::ios::in | std::ios::binary);
    if(!ffile.is_open()) return false;

    unsigned int numMeshes = 0;
    unsigned int numMaterials = 0;

    ffile.read( reinterpret_cast<char*>(&numMeshes), sizeof(unsigned int));
    ffile.read( reinterpret_cast<char*>(&numMaterials), sizeof(unsigned int));

    buffers = new MR::GeometryBuffer*[numMeshes];
    if(log) MR::Log::LogString("Meshes num " + std::to_string(numMeshes), MR_LOG_LEVEL_INFO);
    num = numMeshes;

    for(unsigned int i = 0; i < numMeshes; ++i){
        //Read model name
        unsigned int meshNameLength = 0;
        ffile.read( reinterpret_cast<char*>(&meshNameLength), sizeof(unsigned int));
        if(log) MR::Log::LogString("Mesh name length " + std::to_string(meshNameLength), MR_LOG_LEVEL_INFO);

        char* meshName = new char[meshNameLength];
        if( meshNameLength != 0) {
            ffile.read( &meshName[0], meshNameLength*sizeof(char) );
            if(log) MR::Log::LogString(std::string("Mesh name ") + meshName, MR_LOG_LEVEL_INFO);
        }
        else {
            if(log) MR::Log::LogString("Mesh name is null", MR_LOG_LEVEL_INFO);
            meshName = (char*)&("noname")[0];
        }

        unsigned int materialId = 0;
        ffile.read( reinterpret_cast<char*>(&materialId), sizeof(unsigned int));
        if(log) MR::Log::LogString("Material id " + std::to_string(materialId ), MR_LOG_LEVEL_INFO);

        unsigned int numVerts = 0;
        ffile.read( reinterpret_cast<char*>(&numVerts), sizeof(unsigned int));
        if(log) MR::Log::LogString("Num verts " + std::to_string(numVerts ), MR_LOG_LEVEL_INFO);

        int stride = 0;
        ffile.read( reinterpret_cast<char*>(&stride), sizeof(int));
        if(log) MR::Log::LogString("GL stride " + std::to_string(stride ), MR_LOG_LEVEL_INFO);

        int declarations = 0;
        ffile.read( reinterpret_cast<char*>(&declarations), sizeof(int));
        if(log) MR::Log::LogString("Decl types num " + std::to_string(declarations ), MR_LOG_LEVEL_INFO);

        bool posDecl = false, texCoordDecl = false, normalDecl = false, vertexColorDecl = false;
        for(int j = 0; j < declarations; ++j){
            unsigned char flag = 0;
            ffile.read( reinterpret_cast<char*>(&flag), sizeof(unsigned char));
            if(log) MR::Log::LogString("Flag ", MR_LOG_LEVEL_INFO);

            if(flag == 0) {
                posDecl = true;
                if(log) MR::Log::LogString("Pos", MR_LOG_LEVEL_INFO);
            }
            else if(flag == 1) {
                texCoordDecl = true;
                if(log) MR::Log::LogString("TexCoord", MR_LOG_LEVEL_INFO);
            }
            else if(flag == 2) {
                normalDecl = true;
                if(log) MR::Log::LogString("Normal", MR_LOG_LEVEL_INFO);
            }
            else if(flag == 3) {
                vertexColorDecl = true;
                if(log) MR::Log::LogString("VertexColor", MR_LOG_LEVEL_INFO);
            }
        }

        unsigned int vbufferSize = 0;
        ffile.read( reinterpret_cast<char*>(&vbufferSize), sizeof(unsigned int));
        if(log) MR::Log::LogString("VB size in bytes " + std::to_string(vbufferSize ), MR_LOG_LEVEL_INFO);

        float* vbuffer = new float[vbufferSize];
        ffile.read( reinterpret_cast<char*>(vbuffer), vbufferSize);

        unsigned int facesNum = 0;
        ffile.read( reinterpret_cast<char*>(&facesNum), sizeof(unsigned int));
        if(log) MR::Log::LogString("Faces num " + std::to_string(facesNum ), MR_LOG_LEVEL_INFO);

        unsigned int ibufferSize = 0;
        ffile.read( reinterpret_cast<char*>(&ibufferSize), sizeof(unsigned int));
        if(log) MR::Log::LogString("IB size in bytes " + std::to_string(ibufferSize ), MR_LOG_LEVEL_INFO);

        unsigned int * ibuffer = new unsigned int[facesNum*3];
        ffile.read( reinterpret_cast<char*>(ibuffer), ibufferSize);

        MR::VertexDeclarationType vdtypes[declarations];
        unsigned char idecl = 0;
        int ptr_decl = 0;
        if(posDecl){
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_POSITION, 3, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*3;
        }
        if(texCoordDecl){
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_TEXTURE_COORD, 2, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*2;
        }
        if(normalDecl){
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_NORMAL, 3, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*3;
        }
        if(vertexColorDecl){
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationTypesEnum::VDTE_COLOR, 4, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*4;
        }

        if(log) MR::Log::LogString("Decls " + std::to_string(declarations ), MR_LOG_LEVEL_INFO);

        MR::VertexDeclaration* vDecl = new MR::VertexDeclaration(&vdtypes[0], declarations, GL_FLOAT);
        MR::IndexDeclaration* iDecl = new MR::IndexDeclaration(GL_UNSIGNED_INT);

        buffers[i] = new MR::GeometryBuffer(vDecl, nullptr, &vbuffer[0], vbufferSize, nullptr, 0, numVerts, 0);
        if(bindexes) buffers[i] = new MR::GeometryBuffer(vDecl, iDecl, &vbuffer[0], vbufferSize, &ibuffer[0], ibufferSize, numVerts, facesNum*3);
    }
    if(log) MR::Log::LogString("Imported geometry from ("+file+")", MR_LOG_LEVEL_INFO);

    return true;
}
}
