#include "AssimpImporter.h"
#include <stdlib.h>

MR::GeometryBuffer* ImportGeometryA(const aiScene* s, unsigned int i);

MR::GeometryBuffer* MR::ImportGeometry(std::string file, unsigned int i){
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile( file, aiProcessPreset_TargetRealtime_MaxQuality);

    if( !ai_scene->HasMeshes() )
        return nullptr;

    return ImportGeometryA(ai_scene, i);
}

MR::FullGeometryBuffer* ImportGeometryFullBufferA(const aiScene* ai_scene, unsigned int i){
    std::vector<float> data;
    unsigned int vnum = 0;
    short one_element_verts = ai_scene->mMeshes[i]->mFaces[0].mNumIndices; //2 line / 3 triangle / 4 quad
    //Walk over all faces and add data to "data" array
    for(unsigned int fi = 0; fi < ai_scene->mMeshes[i]->mNumFaces; f++i){
        for(short fii = 0; fii < one_element_verts; fi++i){
            //XYZ position
            data.push_back( ai_scene->mMeshes[i]->mVertices[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].x );
            data.push_back( ai_scene->mMeshes[i]->mVertices[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].y );
            data.push_back( ai_scene->mMeshes[i]->mVertices[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].z );
            //XYZ normal
            data.push_back( ai_scene->mMeshes[i]->mNormals[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].x );
            data.push_back( ai_scene->mMeshes[i]->mNormals[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].y );
            data.push_back( ai_scene->mMeshes[i]->mNormals[ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].z );
            //XY tex coord
            data.push_back( ai_scene->mMeshes[i]->mTextureCoords[0][ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].x );
            data.push_back( ai_scene->mMeshes[i]->mTextureCoords[0][ ai_scene->mMeshes[i]->mFaces[fi].mIndices[fii] ].y );
            vnum++;
        }
    }

    return new MR::FullGeometryBuffer(&data[0], vnum, GL_STATIC_DRAW);
}

MR::GeometryBuffer* ImportGeometryA(const aiScene* ai_scene, unsigned int i){
    std::vector<float> verts;
    for(unsigned int iv = 0; iv < ai_scene->mMeshes[i]->mNumVertices; iv++){
        verts.push_back( ai_scene->mMeshes[i]->mVertices[iv].x );
        verts.push_back( ai_scene->mMeshes[i]->mVertices[iv].y );
        verts.push_back( ai_scene->mMeshes[i]->mVertices[iv].z );
    }

    MR::VertexBuffer* vb = new MR::VertexBuffer(verts.data(), verts.size(), GL_STATIC_DRAW);

    std::vector<unsigned int> inds;

    for(unsigned int ii = 0; ii < ai_scene->mMeshes[i]->mNumFaces; i++i){
        for(unsigned int iin = 0; iin < ai_scene->mMeshes[i]->mFaces[ii].mNumIndices; iin++){
            inds.push_back( ai_scene->mMeshes[i]->mFaces[ii].mIndices[iin] );
        }
    }

    MR::IndexBuffer* ib = new MR::IndexBuffer(inds.data(), inds.size(), GL_STATIC_DRAW);

    if( ai_scene->mMeshes[i]->HasTextureCoords(0) ){
        MR::Log::LogString("import tc");
        float* uvs = new float[ai_scene->mMeshes[i]->mNumVertices*2];

        for(unsigned int iv = 0; iv < ai_scene->mMeshes[i]->mNumVertices*2; iv += 2){
            std::cout << "tc " << ai_scene->mMeshes[i]->mTextureCoords[0][iv/2].x << " " << ai_scene->mMeshes[i]->mTextureCoords[0][iv/2].y << "\n";
            if(iv > 0){
                uvs[iv] = ai_scene->mMeshes[i]->mTextureCoords[0][iv/2].x;
                uvs[iv+1] = ai_scene->mMeshes[i]->mTextureCoords[0][iv/2].y;
            }
            else{
                uvs[iv] = ai_scene->mMeshes[i]->mTextureCoords[0][0].x;
                uvs[iv+1] = ai_scene->mMeshes[i]->mTextureCoords[0][0].y;
            }
            //uvs.push_back( ai_scene->mMeshes[i]->mTextureCoords[0][iv].x );
            //uvs.push_back( ai_scene->mMeshes[i]->mTextureCoords[0][iv].y );
        }

    MR::Log::LogString("ok read tc");
    //&uvs[0], sizeof(float), 12, GL_STATIC_DRAW, GL_TEXTURE_COORD_ARRAY, GL_FLOAT
        std::cout << "float size " << sizeof(float) << " num " << ai_scene->mMeshes[i]->mNumVertices*2 << " num2 " << ai_scene->mMeshes[i]->mNumVertices;
        MR::DataBuffer* uv_db = new MR::DataBuffer(uvs, sizeof(float), ai_scene->mMeshes[i]->mNumVertices*2, GL_STATIC_DRAW, GL_TEXTURE_COORD_ARRAY, GL_FLOAT);
        vb->dataBuffers_num = 1;
        vb->dataBuffers = &uv_db;
        MR::Log::LogString("ok set tc");
    }

    return new MR::GeometryBuffer(vb, ib, GL_TRIANGLES);
}

MR::GeometryBuffer* MR::ImportGeometry(std::string file, std::string name){
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile( file, aiProcessPreset_TargetRealtime_MaxQuality);

    if( !ai_scene->HasMeshes() )
        return nullptr;

    //Find mesh by name
    for(unsigned int i = 0; i < ai_scene->mNumMeshes; ++i){
        if(ai_scene->mMeshes[i]->mName.C_Str() == name.c_str()){
            return ImportGeometryA(ai_scene, i);
        }
    }
    return nullptr;
}

bool FileExists(LPCTSTR fname){
    return ::GetFileAttributes(fname) != DWORD(-1);
}

MR::Mesh* MR::ImportMesh(std::string file){
    struct aiLogStream stream = aiGetPredefinedLogStream(aiDefaultLogStream_STDOUT,NULL);
	aiAttachLogStream(&stream);

    //Get path to file
    std::string file_dir = file.substr(0, file.find_last_of('/')+1);

    //Import scene
    Assimp::Importer importer;
    const aiScene* ai_scene = importer.ReadFile( file, aiProcessPreset_TargetRealtime_MaxQuality);

    if( !ai_scene->HasMeshes() )
        return nullptr;

    MR::Mesh* mesh = new MR::Mesh();
    MR::GeometryBuffer** gbuffers = new MR::GeometryBuffer*[ai_scene->mNumMeshes];
    for(unsigned int i = 0; i < ai_scene->mNumMeshes; ++i){
        //gbuffers[i] = ImportGeometryA(ai_scene, i);
        gbuffers[i] = new MR::GeometryBuffer(nullptr, nullptr, GL_TRIANGLES);
        gbuffers[i]->fullGeometryBuffer = ImportGeometryFullBufferA(ai_scene, i);
    }

    if( ai_scene->HasMaterials()){
        MR::Material** mats = new MR::Material*[ai_scene->mNumMaterials];
        for(unsigned int i = 0; i < ai_scene->mNumMaterials; ++i){
            /*aiString name, texture_base;
            if(AI_SUCCESS != ai_scene->mMaterials[i]->Get(AI_MATKEY_NAME, name));
                MR::Log::LogString(std::string("mat name ")+name.C_Str());
            if(AI_SUCCESS != ai_scene->mMaterials[i]->Get(_AI_MATKEY_TEXTURE_BASE, texture_base));
                MR::Log::LogString(std::string("mat texture ")+texture_base.C_Str());
        */
            aiString path;  // filename
            std::string spath;
            aiReturn texFound = ai_scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &path);
            if (texFound == AI_SUCCESS) {
                spath = path.C_Str();
                if(!FileExists(spath.c_str())) spath = file_dir+spath;
                MR::Log::LogString(std::string("mat texture ")+spath);
            }

            MR::Material* mat = new MR::Material();
            if (texFound == AI_SUCCESS) mat->AmbientTexture = &dynamic_cast<MR::Texture&>( *MR::TextureManager::Instance()->Need(spath));
            mats[i] = mat;
        }

        if(ai_scene->mNumMaterials == 1)
            mesh->SingleMaterial = true;

        //Sort materials for geom buffers
        MR::Material** gmats = new MR::Material*[ai_scene->mNumMeshes];
        for(unsigned int i = 0; i < ai_scene->mNumMeshes; ++i){
            gmats[i] = mats[ai_scene->mMeshes[i]->mMaterialIndex];
        }
        mesh->materials = &gmats[0];
    }

    mesh->geom_buffers = &gbuffers[0];
    mesh->geom_buffers_num = ai_scene->mNumMeshes;

    aiReleaseImport(ai_scene);

    return mesh;
}
