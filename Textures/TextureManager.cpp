#include "TextureManager.hpp"

#include "TextureObjects.hpp"
#include "../Utils/Log.hpp"

#include "../Images/SOILwrapper.hpp"

namespace {

float * NewCheckerImage(size_t const& sizes, float c) {
    float * pixel_data = new float [sizes*sizes];
    for(size_t i = 0; i < (sizes*sizes); ++i){
        pixel_data[i] = c;
    }
    return pixel_data;
}

mr::ImageLoaderSOIL SOIL_image_loader;

}

namespace mr {

template<>
mr::TextureManager mu::StaticSingleton<mr::TextureManager>::_singleton_instance = mr::TextureManager();

ITexture* TextureManager::Create(ITexture::Types const& target) {
    ITexture* texture = static_cast<ITexture*>(new Texture());
    if(!texture->Create(target)) {
        delete texture;
        return nullptr;
    }
    _RegisterTexture(texture);
    return texture;
}

TextureSettings* TextureManager::CreateSettings() {
    TextureSettings* sampler = new TextureSettings();
    if(!sampler->Create()) {
        delete sampler;
        return nullptr;
    }
    _RegisterTextureSampler(sampler);
    return sampler;
}

ITexture* TextureManager::CreateMipmapChecker() {
    ITexture* tex = Create(ITexture::Base2D);

    if(tex == nullptr) return nullptr;

    for(int i = 0; i < 10; ++i) {
        int sz = glm::pow(2, 10-i);
        float* data = NewCheckerImage(sz, (float)(10-i) * 0.1f);
        mr::Log::LogString(std::to_string((float)(10-i) * 0.1f));
        tex->SetData(i, ITexture::DF_RED, ITexture::DT_FLOAT, ITexture::SDF_RGB, sz, sz, 0, &data[0]);
        delete [] data;
    }

    tex->Complete(false);

    return tex;
}

ITexture* TextureManager::FromFile(std::string const& file, IImageLoader::Options const& options) {
    ITexture* tex = Create(ITexture::Base2D);
    if(tex == nullptr) return nullptr;

    IImagePtr image = _imageLoader->Load(file, options);
    if(image == nullptr) {
        delete tex;
        return nullptr;
    }

    const int channelsNum = image->GetChannelsNum();
    ITexture::DataFormat df = ITexture::DataFormat::DF_LUMINANCE;
    if(channelsNum == 1) df = ITexture::DataFormat::DF_RED;
    else if(channelsNum == 3) df = ITexture::DataFormat::DF_RGB;
    else if(channelsNum == 4) df = ITexture::DataFormat::DF_RGBA;

    if(!tex->SetData(0, df, ITexture::DataType::DT_BYTE, ITexture::StorageDataFormat::SDF_RGB8, image->GetWidth(), image->GetHeight(), 0, image->GetData().GetArray())) {
        delete tex;
        return nullptr;
    }
	if(!tex->Complete(true)) {
        delete tex;
        return nullptr;
	}
	return tex;
}

void TextureManager::DestroyAllTextures() {
    for(ITexture* tex : _textures) tex->Destroy();
    _textures.clear();
}

void TextureManager::_RegisterTexture(ITexture* tex) {
    _textures.insert(tex);
}

void TextureManager::_UnRegisterTexture(ITexture* tex) {
    _textures.erase(tex);
}

void TextureManager::_RegisterTextureSampler(TextureSettings* sampler) {
    _textureSamplers.insert(sampler);
}

void TextureManager::_UnRegisterTextureSampler(TextureSettings* sampler) {
    _textureSamplers.erase(sampler);
}

TextureManager::TextureManager() {
    _imageLoader = &SOIL_image_loader;
}

TextureManager::~TextureManager() {
    DestroyAllTextures();
}

}
