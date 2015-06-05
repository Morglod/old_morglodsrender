#include "TextureManager.hpp"
#include "Texture2D.hpp"
#include "../Buffers/BufferManager.hpp"

#include "../Utils/Log.hpp"

namespace mr {

template<>
mr::TextureManager mu::StaticSingleton<mr::TextureManager>::_singleton_instance = mr::TextureManager();

Texture2D* TextureManager::CreateTexture2D(Texture::CreationParams const& params) {
    Texture2D* tex2d = new Texture2D();
    if(!tex2d->Create(params)) {
        mr::Log::LogString("Failed TextureManager::CreateTexture2D. Failed create 2d texture.", MR_LOG_LEVEL_ERROR);
        delete tex2d;
        return nullptr;
    }
    _RegisterTexture(tex2d);
    return tex2d;
}

Texture2D* TextureManager::CreateTexture2D(glm::uvec2 const& size, Texture::DataType const& dataType, Texture::DataFormat const& dataFormat, Texture::StorageDataFormat const& sdf, Texture::CreationParams const& params) {
    Texture2D* tex2d = CreateTexture2D(params);
    if(tex2d == nullptr) {
        mr::Log::LogString("Failed TextureManager::CreateTexture2D. Failed create 2d texture.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    if(!tex2d->Storage(size, dataType, dataFormat, sdf)) {
        mr::Log::LogString("Failed TextureManager::CreateTexture2D. Failed make 2d texture storage.", MR_LOG_LEVEL_ERROR);
        delete tex2d;
        return nullptr;
    }
    return tex2d;
}

Texture2D* TextureManager::LoadTexture2DFromFile(std::string const& file, Texture::CreationParams const& params) {
    Texture2D* tex2d = CreateTexture2D(params);
    if(tex2d == nullptr) {
        mr::Log::LogString("Failed TextureManager::LoadTexture2DFromFile. Failed create 2d texture.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    {
        TextureDataPtr texData = TextureData::FromFile(file);
        if(texData == nullptr) {
            mr::Log::LogString("Failed TextureManager::LoadTexture2DFromFile. Failed load texture data from file \""+file+"\".", MR_LOG_LEVEL_ERROR);
            _UnRegisterTexture(tex2d);
            delete tex2d;
            return nullptr;
        }
        if(!tex2d->SetData(texData, Texture::SDF_RGB)) {
            mr::Log::LogString("Failed TextureManager::LoadTexture2DFromFile. Failed set texture data.", MR_LOG_LEVEL_ERROR);
            _UnRegisterTexture(tex2d);
            delete tex2d;
            return nullptr;
        }
    }
    if(!tex2d->Complete(true)) {
        mr::Log::LogString("Failed TextureManager::LoadTexture2DFromFile. Failed complete texture.", MR_LOG_LEVEL_ERROR);
        _UnRegisterTexture(tex2d);
        delete tex2d;
        return nullptr;
    }
    return tex2d;
}

IGPUBuffer* TextureManager::MakeBindlessTexUbo(IGPUBuffer::Usage const& usage, mu::ArrayHandle<Texture*> const& textures) {
    GPUBufferManager& bufferManager = GPUBufferManager::GetInstance();
    const size_t bufSize = sizeof(uint64_t) * textures.GetNum() * 2;
    IGPUBuffer* ubo = bufferManager.CreateBuffer(usage, bufSize);
    if(ubo == nullptr) {
        mr::Log::LogString("Failed TextureManager::MakeBindlessTexUbo. Failed create gpu buffer.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    uint64_t* residentHandles = new uint64_t[textures.GetNum()*2]; //packed as [handle 8byte, none 8byte]
    Texture** texArray = textures.GetArray();
    bool result = true;
    size_t handle_i = 0;
    for(size_t i = 0; i < textures.GetNum(); ++i) {
        result = result && texArray[i]->GetResidentHandle(residentHandles[handle_i]);
        handle_i += 2;
    }
    if(!result) mr::Log::LogString("One or more textures failed returning resident handle in TextureManager::MakeBindlessTexUbo.", MR_LOG_LEVEL_WARNING);

    mr::IGPUBuffer::IMappedRangePtr mapped = ubo->Map(0, bufSize, mr::IGPUBuffer::IMappedRange::Write | mr::IGPUBuffer::IMappedRange::Unsynchronized | mr::IGPUBuffer::IMappedRange::Invalidate);
    if(mapped == nullptr) {
        if(!ubo->Write(residentHandles, 0, 0, bufSize, nullptr, nullptr)) {
            mr::Log::LogString("Failed TextureManager::MakeBindlessTexUbo. Failed write data to gpu buffer.", MR_LOG_LEVEL_ERROR);
            bufferManager.Delete(ubo);
            delete [] residentHandles;
            return nullptr;
        }
    } else {
        uint64_t* mappedMem = (uint64_t*)(mapped->Get());
        memcpy(mappedMem, residentHandles, bufSize);
        mapped->UnMap();
    }
    delete [] residentHandles;

    return ubo;
}

void TextureManager::DestroyAllTextures() {
    for(Texture* tex : _textures) tex->Destroy();
    _textures.clear();
}

void TextureManager::Delete(Texture*& texture) {
    if(texture == nullptr) return;
    _UnRegisterTexture(texture);
    texture->Destroy();
    delete texture;
    texture = nullptr;
}

void TextureManager::_RegisterTexture(Texture* tex) {
    _textures.insert(tex);
}

void TextureManager::_UnRegisterTexture(Texture* tex) {
    _textures.erase(tex);
}

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
    DestroyAllTextures();
}

}
