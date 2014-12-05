#include "SceneLoader.hpp"

#include "../Utils/Log.hpp"

#include "../Materials/MaterialInterfaces.hpp"
#include "../Materials/Material.hpp"

#include "../Geometry/GeometryFormats.hpp"
#include "../Geometry/GeometryManager.hpp"

#include "../Geometry/Mesh.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <assimp/material.h>

#include <glm/glm.hpp>

namespace mr {


class SceneLoader::Impl {
public:
    Assimp::Importer importer;
    mr::TStaticArray<IGeometry*> _geoms;
    mr::TStaticArray<IMaterial*> _materials;
    mr::TStaticArray<IMesh*> _meshes;

    ~Impl() { importer.FreeScene(); }
};

bool SceneLoader::Import(std::string const& file, bool fast) {
    bool bNoError = true;

    const aiScene* scene = _impl->importer.ReadFile(file, (fast) ? 0 : aiProcessPreset_TargetRealtime_MaxQuality);
    if(!scene) {
        mr::Log::LogString("Assimp error: " + std::string(_impl->importer.GetErrorString()), MR_LOG_LEVEL_ERROR);
        return false;
    }

    _impl->_materials = mr::TStaticArray<IMaterial*>(scene->mNumMaterials);
    for(unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        auto material = scene->mMaterials[i];
        MaterialDescr matDescr;

        {
            aiColor4D colA, colD;
            material->Get(AI_MATKEY_COLOR_AMBIENT, colA);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, colD);
            aiString texColor;
            if(0 != material->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), texColor)) {
                material->Get(AI_MATKEY_TEXTURE_AMBIENT(0), texColor);
            }

            float colAlpha = (colA.a + colD.a) / 2.0f;
            matDescr.texColor = std::string(&texColor.data[0], texColor.length);
            matDescr.colorAmbient = glm::vec4(colA.r, colA.g, colA.b, colAlpha);
            matDescr.colorDiffuse = glm::vec3(colD.r, colD.g, colD.b);
        }

        auto newMat = new mr::DefaultMaterial();
        newMat->Create(matDescr);
        _impl->_materials.At(i) = dynamic_cast<IMaterial*>(newMat);
    }

    _impl->_geoms = mr::TStaticArray<IGeometry*>(scene->mNumMeshes);
    _impl->_meshes = mr::TStaticArray<IMesh*>(scene->mNumMeshes);

    for(unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        auto mesh = scene->mMeshes[i];
        bool bPosA = mesh->HasPositions(), bNormalA = mesh->HasNormals(), bColorA = (mesh->GetNumColorChannels() && mesh->HasVertexColors(0)), bTexCoordA = (mesh->GetNumUVChannels() && mesh->HasTextureCoords(0));

        ///Create vertex format

        size_t attr_num = 0;
        /*if(bPosA)*/ ++attr_num;
        if(bNormalA) ++attr_num;
        if(bColorA) ++attr_num;
        if(bTexCoordA) ++attr_num;

        VertexFormatCustomFixed vf;
        vf.SetAttributesNum(attr_num);

        /*if(bPosA)*/   vf.AddVertexAttribute(VertexAttributeCustom(3, &VertexDataTypeFloat::GetInstance(), MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION).Cache());
        if(bNormalA)    vf.AddVertexAttribute(VertexAttributeCustom(3, &VertexDataTypeFloat::GetInstance(), MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION).Cache());
        if(bColorA)     vf.AddVertexAttribute(VertexAttributeCustom(4, &VertexDataTypeFloat::GetInstance(), MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION).Cache());
        if(bTexCoordA)  vf.AddVertexAttribute(VertexAttributeCustom(2, &VertexDataTypeFloat::GetInstance(), MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION).Cache());

        ///Create index format
        IndexFormatCustom fi(&VertexDataTypeUInt::GetInstance());

        ///Pack vertex data

        size_t posOffset = 0, normalOffset = 0, colorOffset = 0, texcoordOffset = 0;

        /*if(bPosA) posOffset = 0; */
        if(bNormalA)    normalOffset = (size_t)bPosA * sizeof(float) * 3;
        if(bColorA)     colorOffset = ((size_t)bPosA * sizeof(float) * 3) + ((size_t)bNormalA * + sizeof(float) * 3);
        if(bTexCoordA)  texcoordOffset = ((size_t)bPosA * sizeof(float) * 3) + ((size_t)bNormalA * + sizeof(float) * 3) + (size_t)bColorA * sizeof(float) * 4;

        size_t vertexDataSize = mesh->mNumVertices * vf.GetSize();
        unsigned char * vertexData = new unsigned char [vertexDataSize];
        for(size_t it = 0; it < mesh->mNumVertices; ++it) {
            size_t offset = vf.GetSize() * it;
            *((aiVector3D*)&vertexData[offset + posOffset]) = mesh->mVertices[it];
            if(bNormalA) *((aiVector3D*)&vertexData[offset + normalOffset]) = mesh->mNormals[it];
            if(bColorA) *((aiColor4D*)&vertexData[offset + colorOffset]) = mesh->mColors[0][it];
            if(bTexCoordA) *((aiVector2D*)&vertexData[offset + texcoordOffset]) = aiVector2D(mesh->mTextureCoords[0][it].x, 1.0f - mesh->mTextureCoords[0][it].y);
        }

        ///Pack index data
        size_t indexDataSize = mesh->mNumFaces * sizeof(unsigned int) * 3;
        unsigned char * indexData = new unsigned char[indexDataSize];
        for(size_t it = 0; it < mesh->mNumFaces; ++it) {
            size_t offset = sizeof(unsigned int) * 3 * it;
            *((unsigned int*)&indexData[offset]) = mesh->mFaces[it].mIndices[0];
            *((unsigned int*)&indexData[offset+sizeof(unsigned int)]) = mesh->mFaces[it].mIndices[1];
            *((unsigned int*)&indexData[offset+sizeof(unsigned int)*2]) = mesh->mFaces[it].mIndices[2];
        }

        ///Make geometry

        _impl->_geoms.At(i) = mr::GeometryManager::GetInstance()->PlaceGeometry(vf.Cache(), &vertexData[0], mesh->mNumVertices,
                                                                                fi.Cache(), &indexData[0], mesh->mNumFaces*3,
                                                                                mr::IGPUBuffer::Usage::Static, mr::IGeometryBuffer::DrawMode::Triangles);

        delete [] vertexData;
        delete [] indexData;
        if(_impl->_geoms.At(i) == nullptr) bNoError = false;

        _impl->_meshes.At(i) = dynamic_cast<mr::IMesh*>(new Mesh(_impl->_geoms.RangeCopy(i,i+1), _impl->_materials.At(mesh->mMaterialIndex)));
    }

    return bNoError;
}

unsigned int SceneLoader::GetGeometriesNum() {
    return this->_impl->_geoms.GetNum();
}

TStaticArray<IGeometry*> SceneLoader::GetGeometry() {
    return this->_impl->_geoms;
}

unsigned int SceneLoader::GetMaterialsNum() {
    return this->_impl->_materials.GetNum();
}

TStaticArray<IMaterial*> SceneLoader::GetMaterials() {
    return this->_impl->_materials;
}

SceneLoader::SceneLoader() : _impl(new SceneLoader::Impl()) {
}

SceneLoader::~SceneLoader() {
}


}
