#include "GeometryBuffer.hpp"

namespace MR
{
VertexDeclarationType::VertexDeclarationType(VertexDeclarationTypesEnum t, unsigned short e_num, const GLvoid *p) : pointer(p)
{
    this->type = t;
    this->elements_num = e_num;
}

VertexDeclaration::VertexDeclaration(VertexDeclarationType* vdt, unsigned short dn, GLenum dt)
{
    this->_decl_types = vdt;
    this->_decl_types_num = dn;
    this->_data_type = dt;

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
        e_num += _decl_types[i].elements_num;
    }
    this->stride_size *= e_num;
}

VertexDeclaration::~VertexDeclaration()
{
    delete [] this->_decl_types;
    this->_decl_types = nullptr;
}

IndexDeclaration::IndexDeclaration(GLenum dt) : _data_type(dt) {}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, GLuint vb, GLuint ib, unsigned int vnum, unsigned int inum, GLenum drawm)
{
    this->draw_mode = drawm;
    this->_decl = vd;
    this->_idecl = id;
    this->_vertex_buffer = vb;
    this->_index_buffer = ib;
    this->_vertexes_num = vnum;
    this->_indexes_num = inum;
}

GeometryBuffer::GeometryBuffer(VertexDeclaration* vd, IndexDeclaration* id, void* data, size_t data_size, void* idata, size_t idata_size, unsigned int vnum, unsigned int inum, GLenum usage, GLenum iusage, GLenum drawm)
 : _vertexes_num(vnum), _indexes_num(inum), draw_mode(drawm), _decl(vd)
{
    glGenBuffers(1, &this->_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, this->_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, usage);

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

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if(id != nullptr){
        this->_idecl = id;

        glGenBuffers(1, &this->_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->_index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, idata_size, idata, iusage);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

GeometryBuffer::~GeometryBuffer()
{
    this->_decl = nullptr;
    if(this->_vertex_buffer != 0) glDeleteBuffers(1, &this->_vertex_buffer);
    if(this->_index_buffer != 0) glDeleteBuffers(1, &this->_index_buffer);
}
}
//DATA BUFFER
/*
void MR::DataBuffer::Set(void* elements, int element_size, unsigned int num, GLenum mode, GLenum type, GLenum data_type){
	if(this->gl_BUFFER == 0) glDeleteBuffers( 1, &this->gl_BUFFER );

	this->elements_size = num*element_size;
    this->elements_num = num;
	this->gl_BUFFER_TYPE = type;
	this->gl_BUFFER_DATA_TYPE = data_type;
    glGenBuffers( 1, &this->gl_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    glBufferData( GL_ARRAY_BUFFER, this->elements_size, elements, mode );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

void MR::DataBuffer::Bind(){

    //GL_VERTEX_ARRAY +
    //GL_NORMAL_ARRAY +
    //GL_COLOR_ARRAY +
    //GL_TEXTURE_COORD_ARRAY +

    glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    if(this->gl_BUFFER_TYPE == GL_VERTEX_ARRAY){
        glVertexPointer( 3, this->gl_BUFFER_DATA_TYPE, 0, 0 );
        glEnableClientState( GL_VERTEX_ARRAY );
    }
    else if(this->gl_BUFFER_TYPE == GL_NORMAL_ARRAY){
        glNormalPointer( this->gl_BUFFER_DATA_TYPE, 0, 0 );
        glEnableClientState( GL_NORMAL_ARRAY );
    }
    else if(this->gl_BUFFER_TYPE == GL_TEXTURE_COORD_ARRAY){
        glTexCoordPointer( 2, this->gl_BUFFER_DATA_TYPE, 0, 0 );
        glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    }
    else if(this->gl_BUFFER_TYPE == GL_COLOR_ARRAY){
        glColorPointer( 3, this->gl_BUFFER_DATA_TYPE, 0,  0 );
        glEnableClientState( GL_COLOR_ARRAY );
    }
}

MR::DataBuffer::DataBuffer(void* elements, int element_size, unsigned int num, GLenum mode, GLenum type, GLenum data_type){
	this->elements_size = num*element_size;
    this->elements_num = num;
	this->gl_BUFFER_TYPE = type;
	this->gl_BUFFER_DATA_TYPE = data_type;
    glGenBuffers( 1, &this->gl_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    glBufferData( GL_ARRAY_BUFFER, this->elements_size, elements, mode );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

MR::DataBuffer::~DataBuffer(){
	this->elements_size = 0;
    this->elements_num = 0;
    glDeleteBuffers( 1, &this->gl_BUFFER );
}

//VERTEX BUFFER
void MR::VertexBuffer::Bind(){
    glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    glVertexPointer( 3, GL_FLOAT, 0, 0 );
    glEnableClientState( GL_VERTEX_ARRAY );
}

MR::VertexBuffer::VertexBuffer(void* elements, unsigned int num, GLenum mode){
    this->elements_size = num*sizeof(float);
    this->elements_num = num;
    glGenBuffers( 1, &this->gl_BUFFER );
    glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    glBufferData( GL_ARRAY_BUFFER, this->elements_size, elements, mode );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

MR::VertexBuffer::~VertexBuffer(){
	this->elements_size = 0;
    this->elements_num = 0;
    glDeleteBuffers( 1, &this->gl_BUFFER );
    this->dataBuffers = NULL;
    this->dataBuffers_num = 0;
}

//INDEX BUFFER
void MR::IndexBuffer::Bind(){
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->gl_BUFFER );
    glEnableClientState( GL_ELEMENT_ARRAY_BUFFER );
}

MR::IndexBuffer::IndexBuffer(unsigned int * elements, unsigned int num, GLenum mode){
    this->elements_num = num;
    glGenBuffers( 1, &this->gl_BUFFER );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->gl_BUFFER );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, this->elements_num * sizeof(unsigned int), elements, mode );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
}

MR::IndexBuffer::~IndexBuffer(){
    this->elements_num = 0;
    glDeleteBuffers( 1, &this->gl_BUFFER );
}

//FULL GEOM BUFFER
MR::FullGeometryBuffer::FullGeometryBuffer(void* elements, unsigned int num, GLenum mode){
    //this->elements_num = num;
    //this->elements_size = sizeof( float );

    this->gl_array_size = num*8;
    //this->gl_array_start_index;

    glGenVertexArrays(1, &this->gl_va);
	glBindVertexArray(this->gl_va);

	glGenBuffers(1, &this->gl_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, this->gl_BUFFER);

	glBufferData(GL_ARRAY_BUFFER, num*8*sizeof(float), elements, mode);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), 0);
	// Normal vectors
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)(sizeof(aiVector3D)+sizeof(aiVector2D)));
	// Texture coordinates
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2*sizeof(aiVector3D)+sizeof(aiVector2D), (void*)sizeof(aiVector3D));

    //glGenBuffers( 1, &this->gl_BUFFER );
    //glBindBuffer( GL_ARRAY_BUFFER, this->gl_BUFFER );
    //glBufferData( GL_ARRAY_BUFFER, this->elements_num * sizeof(unsigned int), elements, mode );
    //glBindBuffer( GL_ARRAY_BUFFER, 0 );

}

//GEOMETRY BUFFER
void MR::GeometryBuffer::Draw(){
    //!!NODE DRAW CODE----------------------
    glPushMatrix();

    glTranslatef(this->tX, this->tY, this->tZ);
    glRotatef(this->rX, 1, 0, 0);
    glRotatef(this->rY, 0, 1, 0);
    glRotatef(this->rZ, 0, 0, 1);
    glScalef(this->sX, this->sY, this->sZ);
    //!!END FIRST PART-----------------------

    //!!IF CHANGE, CHANGE IN Optimizations.h
    if(this->fullGeometryBuffer == nullptr){
        //Bind data
        this->vertexBuffer->Bind();
        for(unsigned int i = 0; i < this->vertexBuffer->dataBuffers_num; ++i){
            this->vertexBuffer->dataBuffers[i]->Bind();
        }
        //Bind indeces and draw, or simply draw
        if(this->indexBuffer != NULL){
                this->indexBuffer->Bind();
                glDrawElements(
                    this->gl_DRAW_MODE, //mode
                    this->indexBuffer->GetElementsNum(), //count
                    GL_UNSIGNED_INT, //type
                    0 //element array buffer offset
                );
                this->indexBuffer->Unbind();
        }
        else glDrawArrays( this->gl_DRAW_MODE, 0, this->vertexBuffer->GetElementsNum() );

        //Unbind data
        for(unsigned int i = 0; i < this->vertexBuffer->dataBuffers_num; ++i){
            this->vertexBuffer->dataBuffers[i]->Unbind();
        }
        this->vertexBuffer->Unbind();
        glBindBuffer( GL_ARRAY_BUFFER, 0 );
    }
    else{
        glDrawArrays(this->gl_DRAW_MODE, this->fullGeometryBuffer->gl_array_start_index, this->fullGeometryBuffer->gl_array_size);
    }

    //!!NODE DRAW CODE--------------
    for(std::vector<Node*>::iterator it = this->Children.begin(); it != this->Children.end(); it++){
        (*it)->Draw();
    }

    glPopMatrix();
    //!!END NODE DRAWING CODE-------
}

MR::GeometryBuffer::GeometryBuffer(MR::VertexBuffer * vb, MR::IndexBuffer * ib, GLenum dm){
    this->gl_DRAW_MODE = dm;
    this->vertexBuffer = vb;
    this->indexBuffer = ib;
}

MR::GeometryBuffer::~GeometryBuffer(){
    this->vertexBuffer = NULL;
    this->indexBuffer = NULL;
}
*/
