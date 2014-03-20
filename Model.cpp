#include "Model.hpp"
#include "Log.hpp"
#include "GeometryBuffer.hpp"
#include "Material.hpp"
#include "Texture.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <iostream>
#include <fstream>

namespace MR {

ModelLod::ModelLod(MR::Mesh** mm, const unsigned short & mmnum) : meshes(mm), meshes_num(mmnum) {
}

ModelLod::~ModelLod() {
    delete meshes;
}

ModelLod* Model::GetLod(const float & dist) {
    if(lods.size() == 0) return nullptr;
    if(lods.size() == 1) return lods[0];

    for(unsigned short it = 0; it < lods.size(); ++it) {
        if( (dist_step*it) > dist ) {
            if(it == 0) return lods[0];
            else return lods[it-1];
        }
    }
    return lods[lods.size()-1];
}

bool Model::Load() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Model "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source == "") {
        if(this->_resource_manager->GetDebugMessagesState()) {
            MR::Log::LogString("Model "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        }
        this->_loaded = false;
        return false;
    }

    MR::ModelFile* mfl = ModelFile::ImportModelFile(this->_source, true, this->_resource_manager->GetDebugMessagesState());
    if(mfl == nullptr) {
        MR::Log::LogString("Model "+this->_name+" ("+this->_source+") load failed. ModelFile is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    } else {
        //lods = new ModelLod*[1];
        AddLod( new ModelLod(mfl->meshes, mfl->meshes_num) );
    }

    this->_loaded = true;
    return true;
}

void Model::UnLoad() {
    for(ModelLod* mlod : lods) {
        delete mlod;
    }
    lods.clear();
}

Model::Model(ModelManager* manager, std::string name, std::string source) :
    Resource(manager, name, source) {}

Model::~Model() {
}

Resource* ModelManager::Create(const std::string& name, const std::string& source) {
    if(this->_debugMessages) MR::Log::LogString("ModelManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Model * m = new Model(this, name, source);
    this->_resources.push_back(m);
    return m;
}

ModelFile::ModelFile() : meshes(nullptr), meshes_num(0) {}

ModelFile* ModelFile::ImportModelFile(std::string file, bool bindexes, bool log) {
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

    MR::Material** materials = new MR::Material*[NumMaterials];

    unsigned int buffers_mat_ids[NumMeshes];
    MR::GeometryBuffer** buffers = new MR::GeometryBuffer*[NumMeshes];

    for(int i = 0; i < NumMaterials; ++i) {
        //Read name
        int materialNameLength = 0;
        ffile.read( reinterpret_cast<char*>(&materialNameLength), sizeof(int));
        if(log) MR::Log::LogString("Material name length " + std::to_string(materialNameLength), MR_LOG_LEVEL_INFO);

        if(materialNameLength != 0) {
            void* matName = new unsigned char[materialNameLength+1];
            ffile.read( reinterpret_cast<char*>(&((unsigned char*)matName)[0]), sizeof(unsigned char)*materialNameLength);
            for (int ci = 0; ci < materialNameLength; ++ci) {
                ((char*)matName)[ci] = (char)(( (int) ((unsigned char*)matName)  [ci])-127);
            }
            ((char *)matName)[materialNameLength] = '\0';

            if(log) MR::Log::LogString("Material texture file name "+std::string((char*)matName), MR_LOG_LEVEL_INFO);
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

        std::string textureFile = "";
        unsigned char wrapModeU = 0;
        unsigned char wrapModeV = 0;
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

            textureFile = std::string((char*)texName);
            if(log) MR::Log::LogString("Material texture file name "+textureFile, MR_LOG_LEVEL_INFO);

            //Skip flags
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //Skip mapping and operation
            ffile.seekg( sizeof(unsigned char) * 2 , std::ios::cur);

            //Skip texture index
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //Skip texture type
            ffile.seekg( sizeof(unsigned char) * 1 , std::ios::cur);

            //Skip uv index
            ffile.seekg( sizeof(unsigned int) * 1 , std::ios::cur);

            //uv mapping
            ffile.read( reinterpret_cast<char*>(&wrapModeU), sizeof(unsigned char));
            ffile.read( reinterpret_cast<char*>(&wrapModeV), sizeof(unsigned char));
            if(log) MR::Log::LogString("Material texture UV WrapModes "+std::to_string(wrapModeU)+" "+std::to_string(wrapModeV), MR_LOG_LEVEL_INFO);
        }

        materials[i] = new MR::Material(MR::MaterialManager::Instance(), "default");
        MR::Texture* tex = dynamic_cast<MR::Texture*>(MR::TextureManager::Instance()->Need( MR::DirectoryFromFilePath(file) + "/" + textureFile ));
        GLint wmT = GL_CLAMP_TO_EDGE, wmS = GL_CLAMP_TO_EDGE;;
        switch(wrapModeU) {
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
        switch(wrapModeV) {
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

        TextureSettings::Ptr texs = TextureSettings::Create();
        tex->SetSettings(texs);
        texs->SetWrapS((TextureSettings::Wrap)wmS);
        texs->SetWrapT((TextureSettings::Wrap)wmT);

        if(textureFile != ""){
            MR::MaterialPass* mp = new MR::MaterialPass(materials[i]);
            mp->SetAlbedoTexture(tex);
            materials[i]->AddPass(mp);
        }
        else materials[i]->AddPass(new MR::MaterialPass(materials[i]));
    }

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

        buffers_mat_ids[i] = materialId-1;

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

        MR::VertexDeclarationType* vdtypes = (MR::VertexDeclarationType*)malloc(declarations*sizeof(MR::VertexDeclarationType));
        unsigned char idecl = 0;
        int ptr_decl = 0;
        if(posDecl) {
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Position, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*3;
        }
        if(texCoordDecl) {
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::TexCoord, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*2;
        }
        if(normalDecl) {
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Normal, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*3;
        }
        if(vertexColorDecl) {
            vdtypes[idecl] = MR::VertexDeclarationType(MR::VertexDeclarationType::DataType::Color, (void*)(ptr_decl));
            ++idecl;
            ptr_decl += sizeof(float)*4;
        }

        if(log) MR::Log::LogString("Decls " + std::to_string(declarations ), MR_LOG_LEVEL_INFO);

        MR::VertexDeclaration* vDecl = new MR::VertexDeclaration(&vdtypes[0], declarations, VertexDeclaration::DataType::Float);
        MR::IndexDeclaration* iDecl = new MR::IndexDeclaration(IndexDeclaration::DataType::UInt);

        buffers[ buffers_mat_ids[i] ] = new MR::GeometryBuffer(vDecl, nullptr, &vbuffer[0], vbufferSize, nullptr, 0, numVerts, 0);
        if(bindexes) buffers[ buffers_mat_ids[i] ] = new MR::GeometryBuffer(vDecl, iDecl, &vbuffer[0], vbufferSize, &ibuffer[0], ibufferSize, numVerts, indsNum);

        delete meshName;
        delete vbuffer;
        delete ibuffer;
    }

    ffile.close();
    if(log) MR::Log::LogString("Imported geometry from ("+file+")", MR_LOG_LEVEL_INFO);

    if(log) MR::Log::LogString("Packing geometry to model from ("+file+")", MR_LOG_LEVEL_INFO);

    MR::Mesh** mesh = new MR::Mesh*[1];

    mesh[0] = new MR::Mesh(buffers, NumMeshes, materials, NumMaterials);

    mfile->meshes = mesh;
    mfile->meshes_num = 1;

    return mfile;
}

}
