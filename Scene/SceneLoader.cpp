#include "SceneLoader.hpp"

#include "../Utils/Log.hpp"

#include "../Materials/MaterialInterfaces.hpp"
#include "../Materials/Material.hpp"

#include "../Geometry/GeometryFormats.hpp"
#include "../Geometry/GeometryManager.hpp"

#include "../Geometry/Mesh.hpp"

#include "../Textures/TextureManager.hpp"
#include "../MachineInfo.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>

#include <glm/glm.hpp>

#include <iostream>
#include <unordered_map>

namespace mr {

namespace {

class SceneLoaderProgressHandler : public Assimp::ProgressHandler {
public:
    std::function<bool (float percentage)> callback = nullptr;
    bool Update(float percentage) override {
        if(callback != nullptr) return callback(percentage);
        return true;
    }
};

}

class SceneLoader::Impl {
public:
    Assimp::Importer importer;
    mr::TStaticArray<IGeometry*> _geoms;
    mr::TStaticArray<IMaterial*> _materials;
    mr::TStaticArray<IMesh*> _meshes;
    SceneLoaderProgressHandler progressHandler;

    Impl() {
        importer.SetProgressHandler(&progressHandler);
    }

    ~Impl() {
        importer.SetProgressHandler(0);
        importer.FreeScene();
    }
};

namespace {

aiNode* _AssimpFindMeshNode(aiNode* node, unsigned int const& meshIndex) {
    for(unsigned int i = 0; i < node->mNumMeshes; ++i) {
        if(meshIndex == node->mMeshes[i]) return node;
    }
    for(unsigned int i = 0; i < node->mNumChildren; ++i) {
        aiNode* result = _AssimpFindMeshNode(node->mChildren[i], meshIndex);
        if(result) return result;
    }
    return nullptr;
}

aiMatrix4x4 _AssimpGetMat(const aiScene* __restrict__ scene, std::unordered_map<size_t, aiMatrix4x4>* __restrict__ nodesCache, unsigned int const& meshIndex) {
    aiNode* node = _AssimpFindMeshNode(scene->mRootNode, meshIndex);
    aiMatrix4x4 mat;
    if(node) {
        mat = nodesCache->at((size_t)node);
    }
    return mat;
}

void _AssimpCacheNodes(aiNode* __restrict__ node, aiMatrix4x4 const& parentTransf, std::unordered_map<size_t, aiMatrix4x4>* __restrict__ nodesCache) {
    aiMatrix4x4 mat = parentTransf * node->mTransformation;
    nodesCache->insert(std::make_pair<size_t, aiMatrix4x4>((size_t)node, std::move(mat)));
    for(unsigned int i = 0; i < node->mNumChildren; ++i) {
        _AssimpCacheNodes(node->mChildren[i], mat, nodesCache);
    }
}

class _AssimpLogStream : public Assimp::LogStream {
public:
    void write(const char* message) override {
        mr::Log::LogString("\nAssimp: " + std::string(message, strlen(message)-1), MR_LOG_LEVEL_INFO);
    }

    _AssimpLogStream() {}
    virtual ~_AssimpLogStream() {}
};

}

bool SceneLoader::Import(std::string const& file, ImportOptions const& options) {
    bool anyError = false;
    SceneLoader::ProgressInfo progressInfo;

    //Setup log if needed
    _AssimpLogStream logStream;
    if(options.debugLog) {
        Assimp::Logger* assimpLogger = Assimp::DefaultLogger::create(ASSIMP_DEFAULT_LOG_NAME, Assimp::Logger::VERBOSE, aiDefaultLogStream_STDOUT, NULL);
        assimpLogger->attachStream(&logStream, Assimp::Logger::Debugging | Assimp::Logger::Info | Assimp::Logger::Warn | Assimp::Logger::Err);
    }

    //Set flags and import
    unsigned int flags = 0;
    if(options.fast) flags |= aiProcessPreset_TargetRealtime_Fast;
    else flags |= aiProcessPreset_TargetRealtime_MaxQuality;
    if(options.fixInfacingNormals) flags |= aiProcess_FixInfacingNormals;
    if(options.flipUVs) flags |= aiProcess_FlipUVs;
    if(options.generateUVs) flags |= aiProcess_GenUVCoords;
    if(options.transformUVs) flags |= aiProcess_TransformUVCoords;

    _impl->progressHandler.callback = options.assimpProcessCallback;
    const aiScene* scene = _impl->importer.ReadFile(file, flags);

    if(!scene) {
        mr::Log::LogString("Assimp error: " + std::string(_impl->importer.GetErrorString()), MR_LOG_LEVEL_ERROR);
        return false;
    }

    //Process materials
    _impl->_materials = mr::TStaticArray<IMaterial*>(scene->mNumMaterials);
    progressInfo.totalMaterials = scene->mNumMaterials;
    mu::ArrayHandle<MaterialDescr> materialDescriptions(new MaterialDescr[scene->mNumMaterials], scene->mNumMaterials);
    std::vector<Texture*> bindlessTexturesCache;
    mr::TextureManager& textureManager = mr::TextureManager::GetInstance();

    for(unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* material = scene->mMaterials[i];
        MaterialDescr matDescr;
        {
            //Colors
            aiColor4D colA, colD;
            material->Get(AI_MATKEY_COLOR_AMBIENT, colA);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, colD);

            float colAlpha = (colA.a + colD.a) / 2.0f;
            matDescr.colorAmbient = glm::vec4(colA.r, colA.g, colA.b, colAlpha);
            matDescr.colorDiffuse = glm::vec3(colD.r, colD.g, colD.b);

            //Get texture
            aiTextureType textureType;
            if(material->GetTextureCount(aiTextureType_AMBIENT) != 0) textureType = aiTextureType_AMBIENT;
            else if(material->GetTextureCount(aiTextureType_DIFFUSE) != 0) textureType = aiTextureType_DIFFUSE;
            else {
                textureType = aiTextureType_AMBIENT;
                mr::Log::LogString("No AMBIENT or DIFFUSE textures in mateiral.", MR_LOG_LEVEL_WARNING);
            }

            aiString colorTexturePath;
            aiTextureMapping colorTextureMapping;
            unsigned int colorTextureUvIndex = 0;
            float colorTextureBlend = 0.0f;
            aiTextureOp colorTextureOp;
            aiTextureMapMode colorTextureMapMode;

            material->GetTexture(textureType, 0, &colorTexturePath, &colorTextureMapping, &colorTextureUvIndex, &colorTextureBlend, &colorTextureOp, &colorTextureMapMode);
            matDescr.colorTexture.texture = static_cast<mr::Texture*>(textureManager.LoadTexture2DFromFile(std::string(&colorTexturePath.data[0], colorTexturePath.length)));
            matDescr.texColorWrapMode = mr::TextureSettings::Wrap_REPEAT;
            if(colorTextureMapMode == aiTextureMapMode_Wrap) matDescr.texColorWrapMode = mr::TextureSettings::Wrap_REPEAT;
            else if(colorTextureMapMode == aiTextureMapMode_Clamp) matDescr.texColorWrapMode = mr::TextureSettings::Wrap_CLAMP;
            else if(colorTextureMapMode == aiTextureMapMode_Decal) matDescr.texColorWrapMode = mr::TextureSettings::Wrap_DECAL;
            else if(colorTextureMapMode == aiTextureMapMode_Mirror) matDescr.texColorWrapMode = mr::TextureSettings::Wrap_MIRRORED_REPEAT;

            bool arb_bindless;
            if(mr::gl::IsBindlessTextureSupported(arb_bindless)) {
                matDescr.colorTexture.texture->MakeResident();
                bindlessTexturesCache.push_back(matDescr.colorTexture.texture);
            }
        }
        materialDescriptions.GetArray()[i] = matDescr;

        progressInfo.materials = i+1;
        if(options.progressCallback) options.progressCallback(progressInfo);
    }

    IGPUBuffer* bindlessTexturesUBO = nullptr;
    bool arb_bindless;
    if(mr::gl::IsBindlessTextureSupported(arb_bindless)) {
        bindlessTexturesUBO = textureManager.MakeBindlessTexUbo(IGPUBuffer::Static, mu::ArrayHandle<Texture*>(bindlessTexturesCache.data(), bindlessTexturesCache.size(), false, false));
    }

    for(unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        MaterialDescr& matDescr = materialDescriptions.GetArray()[i];
        matDescr.colorTexture.ubo = bindlessTexturesUBO;
        if(bindlessTexturesUBO != nullptr) {
            matDescr.colorTexture.index = i;
        }
        auto newMat = new mr::DefaultMaterial();
        newMat->Create(matDescr);
        _impl->_materials.At(i) = static_cast<IMaterial*>(newMat);
    }

    //Process nodes
    std::unordered_map<size_t, aiMatrix4x4> nodesCache;
    _AssimpCacheNodes(scene->mRootNode, aiMatrix4x4(), &nodesCache);

    _impl->_geoms = mr::TStaticArray<IGeometry*>(scene->mNumMeshes);
    _impl->_meshes = mr::TStaticArray<IMesh*>(scene->mNumMeshes);

    size_t debugVerticiesNum = 0;
    size_t debugIndeciesNum = 0;

    //Process geometry
    GeomDataTypePtr float_DataType = std::make_shared<GeomDataType>(GeomDataType::Float, sizeof(float));
    GeomDataTypePtr uint_DataType = std::make_shared<GeomDataType>(GeomDataType::UInt, sizeof(unsigned int));
    progressInfo.totalMeshes = scene->mNumMeshes;
    for(unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        aiMatrix4x4 transformMatrix = _AssimpGetMat(scene, &nodesCache, i);

        //Attribs flags
        const bool bAttrib[4] = {   mesh->HasPositions(),
                                    mesh->HasNormals(),
                                    (mesh->GetNumColorChannels() && mesh->HasVertexColors(0)),
                                    (mesh->GetNumUVChannels() && mesh->HasTextureCoords(0)) };

        ///Create vertex format

        size_t attributesNum = 0;
        for(unsigned char attr_i = 0; attr_i < 4; ++attr_i) {
            if(bAttrib[attr_i]) ++attributesNum;
        }

        VertexFormatPtr vertexFormatPtr = std::make_shared<VertexFormat>();
        vertexFormatPtr->attributes = mu::ArrayHandle<VertexAttribute>(new VertexAttribute[attributesNum], attributesNum, true, false);
        vertexFormatPtr->pointers = mu::ArrayHandle<uint64_t>(new uint64_t[attributesNum], attributesNum, true, false);
        vertexFormatPtr->attribsNum = attributesNum;

        {
            size_t attr_index = 0;
            if(bAttrib[0]) {
                vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(3, sizeof(float)*3, MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, 0, float_DataType), attr_index);
                attr_index++;
            }
            if(bAttrib[1]) {
                vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(3, sizeof(float)*3, MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, 0, float_DataType), attr_index);
                attr_index++;
            }
            if(bAttrib[2]) {
                vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(4, sizeof(float)*4, MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION, 0, float_DataType), attr_index);
                attr_index++;
            }
            if(bAttrib[3]) {
                vertexFormatPtr->SetAttribute(std::make_shared<VertexAttributeDesc>(2, sizeof(float)*2, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, 0, float_DataType), attr_index);
                attr_index++;
            }
        }

        vertexFormatPtr->Complete();

        ///Create index format
        IndexFormatPtr indexFormatPtr = std::make_shared<IndexFormat>(uint_DataType);

        ///Pack vertex data
        mu::ArrayHandle<uint64_t> const& vertexOffsetsRef = vertexFormatPtr->pointers;
        uint64_t* vertexOffsets = vertexOffsetsRef.GetArray();
        size_t vertexOffsets_attribs_map[4] = {0,0,0,0};

        size_t vertexOffsets_attribs_map_index = 0;
        for(unsigned char attr_i = 0; attr_i < 4; ++attr_i) {
            if(bAttrib[attr_i]) {
                vertexOffsets_attribs_map[attr_i] = vertexOffsets_attribs_map_index++;
            }
        }

        size_t vertexDataSize = mesh->mNumVertices * vertexFormatPtr->size;
        unsigned char * vertexData = new unsigned char [vertexDataSize];
        for(size_t it = 0; it < mesh->mNumVertices; ++it) {
            const size_t vertexOffset = vertexFormatPtr->size * it;

            {
                aiVector3D pos = mesh->mVertices[it];
                pos *= transformMatrix;
                memcpy(&vertexData[vertexOffset + vertexOffsets[vertexOffsets_attribs_map[0]]], &pos, sizeof(float)*3);
            }

            if(bAttrib[1]) memcpy(&vertexData[vertexOffset + vertexOffsets[vertexOffsets_attribs_map[1]]], &(mesh->mNormals[it]),  sizeof(float)*3);
            if(bAttrib[2]) memcpy(&vertexData[vertexOffset + vertexOffsets[vertexOffsets_attribs_map[2]]], &(mesh->mColors[0][it]),sizeof(float)*4);
            if(bAttrib[3]) {
                aiVector2D texCoord(mesh->mTextureCoords[0][it].x, mesh->mTextureCoords[0][it].y);
                memcpy(&vertexData[vertexOffset + vertexOffsets[vertexOffsets_attribs_map[3]]], &texCoord, sizeof(float)*2);
            }
        }

        ///Pack index data
        size_t indexNum = mesh->mNumFaces * 3;
        unsigned int * indexData = new unsigned int[indexNum];
        for(size_t it = 0; it < mesh->mNumFaces; ++it) {
            memcpy(&indexData[it*3], &mesh->mFaces[it].mIndices[0], sizeof(unsigned int)*3);
        }

        ///Make geometry
        debugVerticiesNum += mesh->mNumVertices;
        debugIndeciesNum += mesh->mNumFaces;
        _impl->_geoms.At(i) = mr::GeometryManager::GetInstance()->PlaceGeometry(vertexFormatPtr, &vertexData[0], mesh->mNumVertices,
                                                                                indexFormatPtr, &indexData[0], mesh->mNumFaces*3,
                                                                                mr::IGPUBuffer::Usage::Static, mr::IGeometryBuffer::DrawMode::Triangles);

        delete [] vertexData;
        delete [] indexData;
        if(_impl->_geoms.At(i) == nullptr) anyError = false;

        _impl->_meshes.At(i) = dynamic_cast<mr::IMesh*>(new Mesh(_impl->_geoms.RangeCopy(i,i+1), _impl->_materials.At(mesh->mMaterialIndex)));


        progressInfo.meshes = i+1;
        if(options.progressCallback) options.progressCallback(progressInfo);
    }

    if(options.debugLog) {
        mr::Log::LogString("Total verticies: " + std::to_string(debugVerticiesNum));
        mr::Log::LogString("Total indecies: " + std::to_string(debugIndeciesNum));
    }

    return !anyError;
}

unsigned int SceneLoader::GetGeometriesNum() {
    return _impl->_geoms.GetNum();
}

TStaticArray<IGeometry*> SceneLoader::GetGeometry() {
    return _impl->_geoms;
}

unsigned int SceneLoader::GetMaterialsNum() {
    return _impl->_materials.GetNum();
}

TStaticArray<IMaterial*> SceneLoader::GetMaterials() {
    return _impl->_materials;
}

TStaticArray<IMesh*> SceneLoader::GetMeshes() {
    return _impl->_meshes;
}

SceneLoader::SceneLoader() : _impl(new SceneLoader::Impl()) {
}

SceneLoader::~SceneLoader() {
}


}
