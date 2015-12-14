#include "mr/mesh.hpp"
#include "mr/log.hpp"
#include "mr/texture.hpp"
#include "mr/buffer.hpp"
#include "src/mp.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/color4.h>
#include <assimp/material.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/ProgressHandler.hpp>

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
        MR_LOG("Assimp: " + std::string(message, strlen(message)-1));
    }

    Assimp::Logger* _logger = nullptr;

    void Attach(Assimp::Logger* logger) {
        _logger = logger;
        logger->attachStream(this, Assimp::Logger::Debugging | Assimp::Logger::Info | Assimp::Logger::Warn | Assimp::Logger::Err);
    }

    _AssimpLogStream() {}
    virtual ~_AssimpLogStream() {
        if(_logger != nullptr) {

        }
        _logger = nullptr;
    }
};

mr::Texture2DPtr _AssimpLoadTexture(aiMaterial* material, aiTextureType const& texType, mr::TextureWrap& out_Wrap) {
    aiString path;
    aiTextureMapping mapping;
    unsigned int uvIndex = 0;
    float blend = 0.0f;
    aiTextureOp op;
    aiTextureMapMode mapMode;

    material->GetTexture(texType, 0, &path, &mapping, &uvIndex, &blend, &op, &mapMode);

    mr::Texture2DPtr tex = nullptr;

    const std::string texturePath = std::string(&path.data[0], path.length);

    mr::TextureDataPtr textureData = mr::TextureData::FromFile(texturePath);
    if(textureData == nullptr) {
        MR_LOG_ERROR(_AssimpLoadTexture, "Failed load texture data from file \""+texturePath+"\".");
        return nullptr;
    }

    mr::TextureParams textureParams;
    textureParams.minFilter = mr::TextureMinFilter::LinearMipmapLinear;
    textureParams.magFilter = mr::TextureMagFilter::Linear;

    out_Wrap = mr::TextureWrap::Repeat;
    if(mapMode == aiTextureMapMode_Wrap) out_Wrap = mr::TextureWrap::Repeat;
    else if(mapMode == aiTextureMapMode_Clamp) out_Wrap = mr::TextureWrap::Clamp;
    else if(mapMode == aiTextureMapMode_Decal) out_Wrap = mr::TextureWrap::Decal;
    else if(mapMode == aiTextureMapMode_Mirror) out_Wrap = mr::TextureWrap::MirroredRepeat;

    textureParams.wrapR = textureParams.wrapS = textureParams.wrapT = out_Wrap;

    tex = mr::Texture2D::Create(textureParams);
    tex->Storage();
    tex->WriteImage(textureData);
    tex->BuildMipmaps();
    tex->MakeResident();

    return tex;
}

}

namespace mr {

bool Mesh::Draw(uint32_t instancesNum) {
    MP_ScopeSample(Mesh::Draw);
    _material->UpdateShaderUniforms();
    return _primitive->Draw(_material->GetProgram(), instancesNum);
}

MeshPtr Mesh::Create(PrimitivePtr const& prim, MaterialPtr const& material) {
    MeshPtr m = MeshPtr(new Mesh);
    m->_primitive = prim;
    m->_material = material;
    return m;
}

bool Mesh::Import(std::string const& file, ShaderProgramPtr const& shaderProgram, std::vector<MeshPtr>& out_meshes, ImportOptions options) {
    /// Setup
    bool anyError = false;

    // Log
    _AssimpLogStream logStream;
    if(options.debugLog) {
        Assimp::Logger* assimpLogger = Assimp::DefaultLogger::get();
        if(Assimp::DefaultLogger::isNullLogger()) {
            assimpLogger = Assimp::DefaultLogger::create(ASSIMP_DEFAULT_LOG_NAME, Assimp::Logger::VERBOSE, /*aiDefaultLogStream_STDOUT*/0, nullptr);
            //Assimp::DefaultLogger::set(assimpLogger);
        }
        logStream.Attach(assimpLogger);
    }

    // Flags
    unsigned int flags = 0;
    if(options.fast) flags |= aiProcessPreset_TargetRealtime_Fast;
    else flags |= aiProcessPreset_TargetRealtime_MaxQuality;
    if(options.fixInfacingNormals) flags |= aiProcess_FixInfacingNormals;
    if(options.flipUVs) flags |= aiProcess_FlipUVs;
    if(options.generateUVs) flags |= aiProcess_GenUVCoords;
    if(options.transformUVs) flags |= aiProcess_TransformUVCoords;

    /// Import

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(file, flags);

    if(!scene) {
        MR_LOG_ERROR(Mesh::Import, "Assimp error: " + std::string(importer.GetErrorString()));
        return false;
    }

    /// Process materials

    std::vector<MaterialPtr> materials;
    materials.resize(scene->mNumMaterials, nullptr);
    std::vector<Texture2DPtr> textures;
    textures.resize(scene->mNumTextures, nullptr);

    for(unsigned int i = 0; i < scene->mNumMaterials; ++i) {
        aiMaterial* material = scene->mMaterials[i];
        struct MaterialDesc {
            glm::vec4 colorAmbient;
            glm::vec3 colorDiffuse;
            Texture2DPtr colorTexture;
            Texture2DPtr normalTexture;
            Texture2DPtr specularTexture;
        };
        MaterialDesc matDescr;
        {
            //Colors
            aiColor4D colA, colD;
            material->Get(AI_MATKEY_COLOR_AMBIENT, colA);
            material->Get(AI_MATKEY_COLOR_DIFFUSE, colD);

            float colAlpha = (colA.a + colD.a) / 2.0f;
            matDescr.colorAmbient = glm::vec4(colA.r, colA.g, colA.b, colAlpha);
            matDescr.colorDiffuse = glm::vec3(colD.r, colD.g, colD.b);

            //Get deffuse texture
            bool noDiffuseTexture = false;
            aiTextureType textureType;
            if(material->GetTextureCount(aiTextureType_AMBIENT) != 0) textureType = aiTextureType_AMBIENT;
            else if(material->GetTextureCount(aiTextureType_DIFFUSE) != 0) textureType = aiTextureType_DIFFUSE;
            else {
                noDiffuseTexture = true;
                textureType = aiTextureType_AMBIENT;
                MR_LOG_WARNING(Mesh::Import, "No AMBIENT or DIFFUSE textures in mateiral.");
            }

            if(!noDiffuseTexture) {
                TextureWrap wrapMode;

                //TODO: Check if successfull loaded
                Texture2DPtr tex = _AssimpLoadTexture(material, textureType, wrapMode);
                tex->MakeResident();
                matDescr.colorTexture = tex;
            }

            //Get normal texture
            if(material->GetTextureCount(aiTextureType_NORMALS) != 0) {
                TextureWrap wrapMode;
                Texture2DPtr tex = _AssimpLoadTexture(material, aiTextureType_NORMALS, wrapMode);
                tex->MakeResident();
                matDescr.normalTexture = tex;
            }

            //Get specular texture
            if(material->GetTextureCount(aiTextureType_SHININESS) != 0) {
                TextureWrap wrapMode;
                Texture2DPtr tex = _AssimpLoadTexture(material, aiTextureType_SHININESS, wrapMode);
                tex->MakeResident();
                matDescr.specularTexture = tex;
            }
        }
        // TODO: Pass textures
        materials[i] = Material::Create(shaderProgram, {}, {{"tex", matDescr.colorTexture}});
    }

    /// Process nodes

    std::unordered_map<size_t, aiMatrix4x4> nodesCache;
    _AssimpCacheNodes(scene->mRootNode, aiMatrix4x4(), &nodesCache);

    out_meshes.resize(scene->mNumMeshes, nullptr);

    size_t debugVerticiesNum = 0;
    size_t debugIndeciesNum = 0;

    /// Process geometry

    /// Cache vertex format
    /**
    Vertex format may be the same for all meshes in scene,
    so need to cache it
    **/
    struct sVertexFormat {
        int8_t vertexOffsets_attribs_map[4] = {0,0,0,0};
        bool bAttrib[4] = {false, false, false, false};
        VertexDeclPtr decl = nullptr;
        IndexDataType indexDataType = IndexDataType::UInt;
    };
    std::vector<sVertexFormat> cacheVertexFormat(2); // pre alloc for 2

    for(uint32_t i = 0; i < scene->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[i];
        aiMatrix4x4 transformMatrix = _AssimpGetMat(scene, &nodesCache, i);
        const size_t indexNum = mesh->mNumFaces * 3;

        // Attribs flags
        bool bAttrib[4] = { mesh->HasPositions(),
                            mesh->HasNormals(),
                            (mesh->GetNumColorChannels() && mesh->HasVertexColors(0)),
                            (mesh->GetNumUVChannels() && mesh->HasTextureCoords(0)) };

        sVertexFormat vertexFormat;
        memcpy(&(vertexFormat.bAttrib[0]), &bAttrib[0], sizeof(bool)*4);

        vertexFormat.bAttrib[1] = false; vertexFormat.bAttrib[2] = false;

        /// Create vertex format or get from cache
        for(uint32_t ivf = 0, nvf = cacheVertexFormat.size(); ivf < nvf; ++ivf) {
            // Compare indexDataType here if needed
            if(memcmp(&(cacheVertexFormat[ivf].bAttrib[0]), &(vertexFormat.bAttrib[0]), sizeof(bool)*4) == 0) {
                vertexFormat = cacheVertexFormat[ivf];
            }
        }

        /// Create new
        if(vertexFormat.decl == nullptr) {
            vertexFormat.decl = VertexDecl::Create();
            auto vertexDecl = vertexFormat.decl->Begin();
            if(vertexFormat.bAttrib[0]) vertexDecl.Pos();
            if(vertexFormat.bAttrib[1]) vertexDecl.Normal();
            if(vertexFormat.bAttrib[2]) vertexDecl.Color();
            if(vertexFormat.bAttrib[3]) vertexDecl.Custom(DataType::Float, 2, 0, true);
            vertexDecl.End();

            /// Create index format
            vertexFormat.indexDataType = (indexNum >= USHRT_MAX) ? IndexDataType::UInt : IndexDataType::UShort;

            /// Map attribs indecies
            size_t vertexOffsets_attribs_map_index = 0;
            for(unsigned char attr_i = 0; attr_i < 4; ++attr_i) {
                if(vertexFormat.bAttrib[attr_i]) {
                    vertexFormat.vertexOffsets_attribs_map[attr_i] = vertexOffsets_attribs_map_index++;
                }
            }

            // Cache
            cacheVertexFormat.push_back(vertexFormat);
        }

        /// Pack vertex data
        const auto attribsNum = vertexFormat.decl->GetAttribsNum();
        VertexDecl::Attrib attribs[attribsNum];
        for(int iattrib = 0; iattrib < attribsNum; ++iattrib) {
            attribs[iattrib] = vertexFormat.decl->GetAttribute(iattrib);
        }

        size_t vertexDataSize = mesh->mNumVertices * vertexFormat.decl->GetSize();
        unsigned char * vertexData = new unsigned char [vertexDataSize];
        for(size_t it = 0; it < mesh->mNumVertices; ++it) {
            const size_t vertexOffset = vertexFormat.decl->GetSize() * it;

            {
                aiVector3D pos = mesh->mVertices[it];
                pos *= transformMatrix;
                memcpy(&vertexData[vertexOffset + attribs[vertexFormat.vertexOffsets_attribs_map[0]].offset], &pos, sizeof(float)*3);
            }

            if(vertexFormat.bAttrib[1]) memcpy(&vertexData[vertexOffset + attribs[vertexFormat.vertexOffsets_attribs_map[1]].offset], &(mesh->mNormals[it]),  sizeof(float)*3);
            if(vertexFormat.bAttrib[2]) memcpy(&vertexData[vertexOffset + attribs[vertexFormat.vertexOffsets_attribs_map[2]].offset], &(mesh->mColors[0][it]),sizeof(float)*4);
            if(vertexFormat.bAttrib[3]) {
                aiVector2D texCoord(mesh->mTextureCoords[0][it].x, mesh->mTextureCoords[0][it].y);
                memcpy(&vertexData[vertexOffset + attribs[vertexFormat.vertexOffsets_attribs_map[3]].offset], &texCoord, sizeof(float)*2);
            }
        }

        /// Pack index data
        void * indexData = nullptr;
        if(vertexFormat.indexDataType == IndexDataType::UInt) {
            indexData = malloc(sizeof(uint32_t) *indexNum);
            uint32_t* indexDataM = (uint32_t*)indexData;
            for(size_t it = 0; it < mesh->mNumFaces; ++it) {
                memcpy(&indexDataM[it*3], &mesh->mFaces[it].mIndices[0], sizeof(unsigned int)*3);
            }
        } else if(vertexFormat.indexDataType == IndexDataType::UShort) {
            indexData = malloc(sizeof(uint16_t) *indexNum);
            uint16_t* indexDataM = (uint16_t*)indexData;
            for(size_t it = 0; it < mesh->mNumFaces; ++it) {
                indexDataM[it*3 +0] = (uint16_t) mesh->mFaces[it].mIndices[0];
                indexDataM[it*3 +1] = (uint16_t) mesh->mFaces[it].mIndices[1];
                indexDataM[it*3 +2] = (uint16_t) mesh->mFaces[it].mIndices[2];
            }
        } else {
            MR_LOG_ERROR(Mesh::Import, "Incorrect vertexFormat.indexDataType;");
            return false;
        }

        /// Make geometry
        debugVerticiesNum += mesh->mNumVertices;
        debugIndeciesNum += mesh->mNumFaces;

        BufferPtr vbufferData = Buffer::Create(Memory::Ref(vertexData, vertexDataSize), Buffer::CreationFlags());
        BufferPtr ibufferData = Buffer::Create(Memory::Ref(indexData, indexNum * sizeof(uint32_t)), Buffer::CreationFlags());
        vbufferData->MakeResident(MR_RESIDENT_READ_ONLY);
        ibufferData->MakeResident(MR_RESIDENT_READ_ONLY);

        VertexBufferPtr vbuffer = VertexBuffer::Create(vbufferData, vertexFormat.decl, mesh->mNumVertices);
        IndexBufferPtr ibuffer = IndexBuffer::Create(ibufferData, vertexFormat.indexDataType, indexNum);

        delete [] vertexData;
        free(indexData);

        PrimitivePtr primitve = Primitive::Create(DrawMode::Triangle, vbuffer, ibuffer);
        out_meshes[i] = Mesh::Create(primitve, materials[mesh->mMaterialIndex]);
    }

    if(options.debugLog) {
        MR_LOG("Total verticies: " + std::to_string(debugVerticiesNum));
        MR_LOG("Total indecies: " + std::to_string(debugIndeciesNum));
    }

    return !anyError;
}

}
