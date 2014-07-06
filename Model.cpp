#include "Model.hpp"
#include "Utils/Log.hpp"
#include "Buffers/GeometryBufferV2.hpp"
#include "Material.hpp"
#include "Texture.hpp"
#include "Shaders/ShaderInterfaces.hpp"
#include "Shaders/ShaderBuilder.hpp"
#include "GL/Context.hpp"
#include "Utils/Singleton.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <iostream>
#include <fstream>

#include <map>

namespace MR {

ModelLod::ModelLod(MR::Mesh** mm, const unsigned short & mmnum) : meshes(mm), meshes_num(mmnum) {
}

ModelLod::~ModelLod() {
    delete meshes;
}

ModelLod* Model::GetLod(const float & dist) {
    if(lods.size() == 0) return nullptr;
    if(lods.size() == 1) return lods[0];

    for(size_t it = 0; it < lods.size(); ++it) {
        if( (dist_step*it) > dist ) {
            if(it == 0) return lods[0];
            else return lods[it-1];
        }
    }
    return lods[lods.size()-1];
}

void Model::_UnLoading() {
    //if(_async_unloading_handle.NoErrors()) _async_unloading_handle.End();

    for(ModelLod* mlod : lods) {
        delete mlod;
    }
    lods.clear();
}

bool Model::_Loading() {
    //if(_async_loading_handle.NoErrors()) if(!_async_loading_handle.End()) return false;

    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Model "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source == "") {
        if(this->_resource_manager->GetDebugMessagesState()) {
            MR::Log::LogString("Model "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        }
        return false;
    }

    MR::ModelFile* mfl = ModelFile::ImportModelFile(this->_source, this->_resource_manager->GetDebugMessagesState());
    if(mfl == nullptr) {
        MR::Log::LogString("Model "+this->_name+" ("+this->_source+") load failed. ModelFile is null", MR_LOG_LEVEL_ERROR);
        return false;
    } else {
        //lods = new ModelLod*[1];
        AddLod( new ModelLod(mfl->meshes, mfl->meshes_num) );
    }

    _aabb.ReMake(mfl->minPoint, mfl->maxPoint);
    MR::Log::LogString("Size " + std::to_string(_aabb.GetSize().x) + " " + std::to_string(_aabb.GetSize().y) + " " + std::to_string(_aabb.GetSize().z));

    return true;
}

Model::Model(ResourceManager* manager, std::string name, std::string source) :
    Resource(manager, name, source), _aabb(glm::vec3(0,0,0), glm::vec3(1,1,1)) {}

Model::~Model() {
}

Resource* ModelManager::Create(const std::string& name, const std::string& source) {
    if(this->_debugMessages) MR::Log::LogString("ModelManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Model * m = new Model(dynamic_cast<MR::ResourceManager*>(this), name, source);
    this->_resources.push_back(m);
    return m;
}

SingletonVar(ModelManager, new ModelManager());

ModelManager* ModelManager::Instance() {
    return SingletonVarName(ModelManager).Get();
}

void ModelManager::DestroyInstance() {
    SingletonVarName(ModelManager).Destroy();
}

ModelFile::ModelFile() : meshes(nullptr), meshes_num(0) {}

ModelFile* ModelFile::ImportModelFile(std::string file, bool log, const MR::ModelFile::ImportSettings& isettings) {
    if(log) MR::Log::LogString("Importing geometry from ("+file+")", MR_LOG_LEVEL_INFO);
    std::ifstream ffile(file, std::ios::in | std::ios::binary);
    if(!ffile.is_open()) {
        if(log) MR::Log::LogString("Failed importing geometry from ("+file+") failed openning file", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    ModelFile* mfile = new ModelFile();

    int NumAnims = 0;
    ffile.read( reinterpret_cast<char*>(&NumAnims), sizeof(int));
    if(log) MR::Log::LogString("Anims num " + std::to_string(NumAnims), MR_LOG_LEVEL_INFO);

    int NumCams = 0;
    ffile.read( reinterpret_cast<char*>(&NumCams), sizeof(int));
    if(log) MR::Log::LogString("Cams num " + std::to_string(NumCams), MR_LOG_LEVEL_INFO);

    int NumLights = 0;
    ffile.read( reinterpret_cast<char*>(&NumLights), sizeof(int));
    if(log) MR::Log::LogString("Lights num " + std::to_string(NumLights), MR_LOG_LEVEL_INFO);

    int NumMaterials = 0;
    ffile.read( reinterpret_cast<char*>(&NumMaterials), sizeof(int));
    if(log) MR::Log::LogString("Materials num " + std::to_string(NumMaterials), MR_LOG_LEVEL_INFO);

    int NumMeshes = 0;
    ffile.read( reinterpret_cast<char*>(&NumMeshes), sizeof(int));
    if(log) MR::Log::LogString("Meshes num " + std::to_string(NumMeshes), MR_LOG_LEVEL_INFO);

    int NumTextures = 0;
    ffile.read( reinterpret_cast<char*>(&NumTextures), sizeof(int));
    if(log) MR::Log::LogString("Textures num " + std::to_string(NumTextures), MR_LOG_LEVEL_INFO);

    //Each mesh represented as one material and many geometry buffers

    struct MeshContainer {
        Material* mat;
        std::vector<float> combine_vertdata;
        std::vector<unsigned int> combine_inddata;

        std::vector<IGeometryBuffer*> buffers;
        std::vector<unsigned int> vbuffer_offsets;
        std::vector<unsigned int> ibuffer_offsets;
        std::vector<unsigned int> num_verts;
        std::vector<unsigned int> num_inds;
        MeshContainer(Material* m) : mat(m), buffers() {}
    };

    std::vector<MeshContainer> for_meshes;

    for(int i = 0; i < NumMaterials; ++i) {
        //Read name
        int materialNameLength = 0;
        ffile.read( reinterpret_cast<char*>(&materialNameLength), sizeof(int));
        if(log) MR::Log::LogString("Material name length " + std::to_string(materialNameLength), MR_LOG_LEVEL_INFO);

        std::string materialName = "unknown";

        if(materialNameLength != 0) {
            void* matName = new unsigned char[materialNameLength+1];
            ffile.read( reinterpret_cast<char*>(&((unsigned char*)matName)[0]), sizeof(unsigned char)*materialNameLength);
            for (int ci = 0; ci < materialNameLength; ++ci) {
                ((char*)matName)[ci] = (char)(( (int) ((unsigned char*)matName)  [ci])-127);
            }
            ((char *)matName)[materialNameLength] = '\0';

            materialName = std::string((char*)matName);

            if(log) MR::Log::LogString("Material name "+materialName, MR_LOG_LEVEL_INFO);
        }

        unsigned char blendMode = 0;
        ffile.read( reinterpret_cast<char*>(&blendMode), sizeof(unsigned char));

        //Skip 6 rgba colors
        ffile.seekg( sizeof(float) * 6 * 4, std::ios::cur);//ffile.seekg( ffile.tellg() + ( sizeof(float) * 6 * 4 ) );

        //Skip "is two sided" and "wireframe"
        ffile.seekg( sizeof(unsigned char) * 2, std::ios::cur);

        //Skip opacity and reflectivity
        ffile.seekg( sizeof(float) * 2, std::ios::cur);

        //Skip shading mode
        ffile.seekg( sizeof(unsigned char) * 1, std::ios::cur);

        //Skip Shininess and ShininessStreight
        ffile.seekg( sizeof(float) * 2, std::ios::cur);

        int TexturesNum = 0;
        ffile.read( reinterpret_cast<char*>(&TexturesNum), sizeof(int));
        if(log) MR::Log::LogString("Textures num "+std::to_string(TexturesNum), MR_LOG_LEVEL_INFO);

        //diffuse only

        struct TextureContainer {
            std::string file;
            unsigned char type;
            unsigned char wrapModeU;
            unsigned char wrapModeV;
            TextureContainer(const std::string& f, const unsigned char& t, const unsigned char& wu, const unsigned char& wv) : file(f), type(t), wrapModeU(wu), wrapModeV(wv) {}
        };

        std::vector<TextureContainer> textures;

        for(int ti = 0; ti < TexturesNum; ++ti) {
            float blendFactor = 0.0f;
            ffile.read( reinterpret_cast<char*>(&blendFactor), sizeof(float));

            int textureFileLength = 0;
            ffile.read( reinterpret_cast<char*>(&textureFileLength), sizeof(int));
            if(log) MR::Log::LogString("Texture name length " + std::to_string(textureFileLength), MR_LOG_LEVEL_INFO);

            void* texName = new unsigned char[textureFileLength+1];
            ffile.read( reinterpret_cast<char*>(&((unsigned char*)texName)[0]), sizeof(unsigned char)*textureFileLength);
            for (int ci = 0; ci < textureFileLength; ++ci) {
                ((char*)texName)[ci] = (char)(( (int) ((unsigned char*)texName)  [ci])-127);
            }
            ((char *)texName)[textureFileLength] = '\0';

            std::string textureFile = std::string((char*)texName);
            if(log) MR::Log::LogString("Material texture file name "+textureFile, MR_LOG_LEVEL_INFO);

            //Skip flags
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //Skip mapping and operation
            ffile.seekg( sizeof(unsigned char) * 2 , std::ios::cur);

            //Skip texture index
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //Read texture type
            unsigned char texType = 0;
            ffile.read( reinterpret_cast<char*>(&texType), sizeof(char));

            //Skip uv index
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //uv mapping
            unsigned char wrapModeU = 0;
            unsigned char wrapModeV = 0;
            ffile.read( reinterpret_cast<char*>(&wrapModeU), sizeof(unsigned char));
            ffile.read( reinterpret_cast<char*>(&wrapModeV), sizeof(unsigned char));
            if(log) MR::Log::LogString("Material texture UV WrapModes "+std::to_string(wrapModeU)+" "+std::to_string(wrapModeV), MR_LOG_LEVEL_INFO);

            textures.push_back(TextureContainer(textureFile, texType, wrapModeU, wrapModeV));
        }

        MR::Material* matPtr = new MR::Material(MR::MaterialManager::Instance(), materialName);
        MR::MaterialPass* matPassPtr = matPtr->CreatePass();
        MR::MaterialPass* matPassPtr_rtt = matPtr->CreatePass();
        *(matPassPtr_rtt->GetFlagPtr()) = MaterialFlag::ToTexture();
        for_meshes.push_back(MeshContainer(matPtr));

        MR::IShaderProgram::Features shaderRequest;

        for(auto it = textures.begin(); it != textures.end(); ++it){
            MR::Texture* tex = nullptr;
            if( (it->type != 1) && (it->type != 2) && (it->type != 9) ) continue;
            if(!isettings.ambientTextures && (it->type == 1)) continue;
            if(it->file != "") {

                tex = dynamic_cast<MR::Texture*>(MR::TextureManager::Instance()->Need( MR::DirectoryFromFilePath(file) + "/" + it->file ));
                GLint wmT = GL_CLAMP_TO_EDGE, wmS = GL_CLAMP_TO_EDGE;
                switch(it->wrapModeU) {
                case 1: //clamp
                    wmT = GL_CLAMP_TO_EDGE;
                    break;
                case 2: //decal
                    wmT = GL_DECAL;
                    break;
                case 3: //mirror
                    wmT = GL_MIRRORED_REPEAT;
                    break;
                case 4: //wrap
                    wmT = GL_REPEAT;
                    break;
                }
                switch(it->wrapModeV) {
                case 1: //clamp
                    wmS = GL_CLAMP_TO_EDGE;
                    break;
                case 2: //decal
                    wmS = GL_DECAL;
                    break;
                case 3: //mirror
                    wmS = GL_MIRRORED_REPEAT;
                    break;
                case 4: //wrap
                    wmS = GL_REPEAT;
                    break;
                }

                TextureSettings* texs = new TextureSettings();
                tex->SetSettings(texs);
                texs->SetWrapS((ITextureSettings::Wrap)wmS);
                texs->SetWrapT((ITextureSettings::Wrap)wmT);

                switch(it->type){
                case 1: //ambient
                    matPtr->SetAmbientTexture(tex);
                    shaderRequest.ambient = true;
                    break;
                case 2: //diffuse
                    matPtr->SetDiffuseTexture(tex);
                    shaderRequest.diffuse = true;
                    break;
                case 9: //opacity
                    matPtr->SetOpacityTexture(tex);
                    shaderRequest.opacity = true;
                    break;
                }
            }
        }

        matPassPtr->SetShader( MR::ShaderBuilder::Need(shaderRequest) );
        shaderRequest.toRenderTarget = true;
        shaderRequest.defferedRendering = true;
        matPassPtr_rtt->SetShader( MR::ShaderBuilder::Need(shaderRequest) );
    }

    glm::vec3 MinPoint, MaxPoint;

    for(int i = 0; i < NumMeshes; ++i) {
        //Read model name
        unsigned int meshNameLength = 0;
        ffile.read( reinterpret_cast<char*>(&meshNameLength), sizeof(unsigned int));
        if(log) MR::Log::LogString("Mesh name length " + std::to_string(meshNameLength), MR_LOG_LEVEL_INFO);

        char* meshName = new char[meshNameLength];
        if( meshNameLength != 0) {
            ffile.read( &meshName[0], meshNameLength*sizeof(char) );
            if(log) MR::Log::LogString(std::string("Mesh name ") + meshName, MR_LOG_LEVEL_INFO);
        } else {
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
        for(int j = 0; j < declarations; ++j) {
            unsigned char flag = 0;
            ffile.read( reinterpret_cast<char*>(&flag), sizeof(unsigned char));
            if(log) MR::Log::LogString("Flag ", MR_LOG_LEVEL_INFO);

            if(flag == 0) {
                posDecl = true;
                if(log) MR::Log::LogString("Pos", MR_LOG_LEVEL_INFO);
            } else if(flag == 1) {
                texCoordDecl = true;
                if(log) MR::Log::LogString("TexCoord", MR_LOG_LEVEL_INFO);
            } else if(flag == 2) {
                normalDecl = true;
                if(log) MR::Log::LogString("Normal", MR_LOG_LEVEL_INFO);
            } else if(flag == 3) {
                vertexColorDecl = true;
                if(log) MR::Log::LogString("VertexColor", MR_LOG_LEVEL_INFO);
            }
        }

        unsigned int vbufferSize = 0;
        ffile.read( reinterpret_cast<char*>(&vbufferSize), sizeof(unsigned int));
        if(log) MR::Log::LogString("VB size in bytes " + std::to_string(vbufferSize ), MR_LOG_LEVEL_INFO);

        float* vbuffer = new float[vbufferSize];
        ffile.read( reinterpret_cast<char*>(vbuffer), vbufferSize);

        unsigned int indsNum = 0;
        ffile.read( reinterpret_cast<char*>(&indsNum), sizeof(unsigned int));
        if(log) MR::Log::LogString("Indecies num " + std::to_string(indsNum), MR_LOG_LEVEL_INFO);

        unsigned int ibufferSize = 0;
        ffile.read( reinterpret_cast<char*>(&ibufferSize), sizeof(unsigned int));
        if(log) MR::Log::LogString("IB size in bytes " + std::to_string(ibufferSize ), MR_LOG_LEVEL_INFO);

        unsigned int * ibuffer = new unsigned int[indsNum];
        ffile.read( reinterpret_cast<char*>(ibuffer), ibufferSize);

        size_t attribs_num = 0;
        if(posDecl) ++attribs_num;
        if(texCoordDecl) ++attribs_num;
        if(normalDecl) ++attribs_num;
        if(vertexColorDecl) ++attribs_num;

        VertexFormatCustomFixed* vformat = new VertexFormatCustomFixed();
        vformat->SetAttributesNum(attribs_num);

        if(posDecl) vformat->AddVertexAttribute(new VertexAttributeCustom(3, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_Position));
        if(texCoordDecl) vformat->AddVertexAttribute(new VertexAttributeCustom(2, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_TexCoord));
        if(normalDecl) vformat->AddVertexAttribute(new VertexAttributeCustom(3, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_Normal));
        if(vertexColorDecl) vformat->AddVertexAttribute(new VertexAttributeCustom(4, VertexDataTypeFloat::Instance(), IVertexAttribute::ShaderIndex_Color));

        if(log) MR::Log::LogString("Decls " + std::to_string(declarations ), MR_LOG_LEVEL_INFO);

        //Make min/max points
        //If it's first mesh
        if(i == 0) {
            MinPoint = glm::vec3(vbuffer[0], vbuffer[1], vbuffer[2]);
            MaxPoint = glm::vec3(vbuffer[0], vbuffer[1], vbuffer[2]);
        }

        //Walk over all positions and find min/max
        for(size_t ipm = 0; ipm < vbufferSize; ipm += (vformat->Size() / sizeof(float))){
            if(vbuffer[ipm] < MinPoint.x) MinPoint.x = vbuffer[ipm];
            if(vbuffer[ipm+1] < MinPoint.y) MinPoint.y = vbuffer[ipm+1];
            if(vbuffer[ipm+2] < MinPoint.z) MinPoint.z = vbuffer[ipm+2];

            if(vbuffer[ipm] > MaxPoint.x) MaxPoint.x = vbuffer[ipm];
            if(vbuffer[ipm+1] > MaxPoint.y) MaxPoint.y = vbuffer[ipm+1];
            if(vbuffer[ipm+2] > MaxPoint.z) MaxPoint.z = vbuffer[ipm+2];
        }

        //MR::VertexDeclaration* vDecl = new MR::VertexDeclaration(&vdtypes[0], declarations, VertexDeclaration::DataType::Float);
        //MR::IndexDeclaration* iDecl = new MR::IndexDeclaration(IndexDeclaration::DataType::UInt);

        /*VertexBuffer* gl_v_buffer = new VertexBuffer(GL::GetCurrent());
        gl_v_buffer->Buffer(&vbuffer[0], vbufferSize, IBuffer::Static+IBuffer::Draw, IBuffer::ReadOnly);
        gl_v_buffer->SetNum(numVerts);
*/
        IndexFormatCustom* gl_i_format = nullptr;
/*        IndexBuffer* gl_i_buffer = nullptr;*/

        if(isettings.indecies) {
            gl_i_format = new IndexFormatCustom(VertexDataTypeUInt::Instance());
/*
            gl_i_buffer = new IndexBuffer(GL::GetCurrent());
            gl_i_buffer->Buffer(&ibuffer[0], ibufferSize, IBuffer::Static+IBuffer::Draw, IBuffer::ReadOnly);
            gl_i_buffer->SetNum(indsNum);*/
        }

        unsigned int vbuf_off_bytes = 0, ibuf_off_bytes = 0;
        MR::GeometryBuffer* new_gb = MR::GeometryManager::Instance()->PlaceGeometry(vformat, gl_i_format, &vbuffer[0], vbufferSize, &ibuffer[0], ibufferSize, IBuffer::Static+IBuffer::Draw, IBuffer::ReadOnly, GL_TRIANGLES, &vbuf_off_bytes, &ibuf_off_bytes);//new GeometryBuffer(GL::GetCurrent(), gl_v_buffer, gl_i_buffer, vformat, gl_i_format, GL_TRIANGLES);
        if(new_gb != 0){
            vbuf_off_bytes = vbuf_off_bytes / vformat->Size();

            if(gl_i_format) {
                ibuf_off_bytes = ibuf_off_bytes / gl_i_format->Size();
            }

            for_meshes[materialId-1].buffers.push_back(new_gb);
            for_meshes[materialId-1].vbuffer_offsets.push_back(vbuf_off_bytes);
            for_meshes[materialId-1].ibuffer_offsets.push_back(ibuf_off_bytes);

            for_meshes[materialId-1].num_verts.push_back(numVerts);
            for_meshes[materialId-1].num_inds.push_back(indsNum);
        }

        delete meshName;
        delete vbuffer;
        delete ibuffer;
    }

    ffile.close();
    if(log) MR::Log::LogString("Imported geometry from ("+file+")", MR_LOG_LEVEL_INFO);

    if(log) MR::Log::LogString("Packing geometry to model from ("+file+")", MR_LOG_LEVEL_INFO);

    MR::Mesh** meshes = new MR::Mesh*[for_meshes.size()];

    for(size_t mi = 0; mi < for_meshes.size(); ++mi){
        MR::IGeometry** mesh_geometry = new MR::IGeometry*[for_meshes[mi].buffers.size()];
        for(size_t gmi = 0; gmi < for_meshes[mi].buffers.size(); ++gmi){
            unsigned int ibegin = 0, vbegin = 0;
            unsigned int icount = 0, vcount = 0;
            if(for_meshes[mi].buffers[gmi]->GetIndexBuffer()){
                icount = for_meshes[mi].num_inds[gmi];
                ibegin = for_meshes[mi].ibuffer_offsets[gmi];
            } else if(for_meshes[mi].buffers[gmi]->GetVertexBuffer()){
                vcount = for_meshes[mi].num_verts[gmi];
                vbegin = for_meshes[mi].vbuffer_offsets[gmi];
            }
            mesh_geometry[gmi] = new MR::Geometry(for_meshes[mi].buffers[gmi], ibegin, vbegin, icount, vcount);
        }
        meshes[mi] = new MR::Mesh(mesh_geometry, for_meshes[mi].buffers.size(), for_meshes[mi].mat);
    }

    mfile->minPoint = MinPoint;
    mfile->maxPoint = MaxPoint;
    mfile->meshes = meshes;
    mfile->meshes_num = for_meshes.size();

    return mfile;
}

}
