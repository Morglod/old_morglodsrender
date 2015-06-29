#include "TextureManager.hpp"
#include "Texture2D.hpp"
#include "TextureUBO.hpp"

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

TextureUBO* TextureManager::MakeTextureUbo(mu::ArrayHandle<Texture*> const& textures, const bool fastChange) {
    TextureUBO* textureUbo = new TextureUBO();
    if(!textureUbo->Init(textures, fastChange)) {
        delete textureUbo;
        return nullptr;
    }

    _RegisterUBO(textureUbo);
    return textureUbo;
}

void TextureManager::DestroyAllTextures() {
    for(Texture* tex : _textures) tex->Destroy();
    _textures.clear();
}

void TextureManager::DestroyAllUBOs() {
    for(TextureUBO* tex : _ubos) tex->Destroy();
    _ubos.clear();
}

void TextureManager::Delete(Texture*& texture) {
    if(texture == nullptr) return;
    _UnRegisterTexture(texture);
    texture->Destroy();
    delete texture;
    texture = nullptr;
}

void TextureManager::Delete(TextureUBO*& textureUbo) {
    if(textureUbo == nullptr) return;
    _UnRegisterUBO(textureUbo);
    textureUbo->Destroy();
    delete textureUbo;
    textureUbo = nullptr;
}

void TextureManager::_RegisterTexture(Texture* tex) {
    _textures.insert(tex);
}

void TextureManager::_UnRegisterTexture(Texture* tex) {
    _textures.erase(tex);
}

void TextureManager::_RegisterUBO(TextureUBO* tex) {
    _ubos.insert(tex);
}

void TextureManager::_UnRegisterUBO(TextureUBO* tex){
    _ubos.erase(tex);
}

TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
    DestroyAllTextures();
}

}
