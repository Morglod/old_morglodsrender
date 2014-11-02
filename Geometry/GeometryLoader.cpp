#include "GeometryLoader.hpp"

#include "../Utils/Log.hpp"
#include "GeometryFormats.hpp"
#include "GeometryManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>

#include <glm/glm.hpp>

namespace MR {

class GeometryLoader::Impl {
public:
    Assimp::Importer importer;
    MR::TStaticArray<IGeometry*> _geoms;

    ~Impl() { importer.FreeScene(); }
};

bool GeometryLoader::Import(std::string const& file) {
    bool bNoError = true;

    const aiScene* scene = _impl->importer.ReadFile(file, aiProcessPreset_TargetRealtime_MaxQuality);
    if(!scene) {
        MR::Log::LogString("Assimp error: " + std::string(_impl->importer.GetErrorString()), MR_LOG_LEVEL_ERROR);
        return false;
    }

    _impl->_geoms = MR::TStaticArray<IGeometry*>(new IGeometry*[scene->mNumMeshes], scene->mNumMeshes, true);
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

        IGeometry* geom = MR::GeometryManager::GetInstance()->PlaceGeometry(vf.Cache(), &vertexData[0], mesh->mNumVertices,
                                                                            fi.Cache(), &indexData[0], mesh->mNumFaces*3,
                                                                            MR::IGPUBuffer::Usage::Static, MR::IGeometryBuffer::DrawModes::Triangles);

        delete [] vertexData;
        delete [] indexData;
        _impl->_geoms.At(i) = geom;
        if(geom == nullptr) bNoError = false;
    }

    return bNoError;
}

unsigned int GeometryLoader::GetGeometriesNum() {
    return this->_impl->_geoms.GetNum();
}

TStaticArray<IGeometry*> GeometryLoader::GetGeometry() {
    return this->_impl->_geoms;
}

GeometryLoader::GeometryLoader() : _impl(new GeometryLoader::Impl()) {
}

GeometryLoader::~GeometryLoader() {
}

}

