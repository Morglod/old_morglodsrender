#include "TextureResource.hpp"
#include "TextureLoader.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../RenderManager.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

bool MR::TextureResource::_Loading() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") cpu loading", MR_LOG_LEVEL_INFO);
    if(this->_source == "") {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        return false;
    }
/*
    MR::TextureLoader::TextureLoadFormat f;
    _async_data = MR::TextureLoader::GetInstance()->LoadImage(this->_source, (int*)&gl_width, (int*)&gl_height, &f, MR::TextureLoader::LFO_Auto);
    if((gl_width == 0)||(gl_height == 0)) {
        MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") loading failed. BAD SIZES", MR_LOG_LEVEL_ERROR);

        return false;
    }

    _async_iform = 0;
    _async_form = 0;
    ITexture::CompressionMode cmode = dynamic_cast<TextureManager*>(this->_resource_manager)->GetCompressionMode();

    switch( f ) {
    case TextureLoader::LFO_Luminous:
        _async_iform = GL_R8;
        _async_form = MR::ITexture::Format_LUMINANCE;
        if(cmode != ITexture::CompressionMode::NoCompression) _async_iform = GL_COMPRESSED_LUMINANCE;

        break;
    case TextureLoader::LFO_LuminousAplha:
        _async_iform = GL_RG8;
        _async_form = MR::ITexture::Format_LUMINANCE_ALPHA;
        if(cmode != ITexture::CompressionMode::NoCompression) _async_iform = GL_COMPRESSED_LUMINANCE_ALPHA;

        break;
    case TextureLoader::LFO_RGB:
        _async_iform = MR::ITexture::IFormat_RGB8;
        _async_form = MR::ITexture::Format_RGB;
        if(cmode == ITexture::CompressionMode::DefaultCompression) _async_iform = GL_COMPRESSED_RGB;
        if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGB8_ETC2;

        break;
    case TextureLoader::LFO_RGBA:
        _async_iform = MR::ITexture::IFormat_RGBA8;
        _async_form = MR::ITexture::Format_RGBA;
        if(cmode == ITexture::CompressionMode::DefaultCompression) _async_iform = GL_COMPRESSED_RGBA;
        if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGBA8_ETC2_EAC;

        break;
    default:
        _async_iform = MR::ITexture::IFormat_RGB8;
        _async_form = MR::ITexture::Format_RGB;
        if(cmode == ITexture::CompressionMode::DefaultCompression) _async_iform = GL_COMPRESSED_RGB;
        if(cmode == ITexture::CompressionMode::S3TC) _async_iform = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
        if(cmode == ITexture::CompressionMode::ETC2) _async_iform = GL_COMPRESSED_RGB8_ETC2;

        break;
    }

    glGenTextures(1, &gl_texture);

    MR::RenderManager* rm = MR::RenderManager::GetInstance();
    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, gl_texture, 0);//glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, gl_texture);

    if(MR::MachineInfo::IsTextureStorageSupported()) {
        const int mip_map_levels = 1;
        glTexStorage2D(GL_TEXTURE_2D, mip_map_levels, _async_iform, gl_width, gl_height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gl_width, gl_height, _async_form, GL_UNSIGNED_BYTE, &_async_data[0]);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        glTexImage2D(GL_TEXTURE_2D, 0, _async_iform, gl_width, gl_height, 0, _async_form, GL_UNSIGNED_BYTE, &_async_data[0]);
    }
    glGenerateMipmap(GL_TEXTURE_2D);

    rm->SetTexture(&unbindedTexture); //glBindTexture(GL_TEXTURE_2D, 0);

    if(_async_data) delete [] _async_data;
*/
        this->gl_texture = MR::TextureLoader::GetInstance()->LoadGLTexture(this->_source);
    ResetInfo();

    return true;
}

void MR::TextureResource::_UnLoading() {
    if(_res_free_state) {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data", MR_LOG_LEVEL_INFO);
        glDeleteTextures(1, &this->gl_texture);
    } else {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Texture "+this->_name+" ("+this->_source+") -> ResFreeState is off", MR_LOG_LEVEL_INFO);
    }
    this->gl_texture = 0;
}

TextureResource::TextureResource(TextureManager* manager, const std::string& name, const std::string& file, const unsigned int& handle, const MR::ITexture::Target& target) : Resource(dynamic_cast<MR::ResourceManager*>(manager), name, file), Texture(handle, target) {
}

TextureResource::~TextureResource() {
    UnLoad();
}

MR::TextureResource* MR::TextureResource::FromFile(MR::TextureManager* m, const std::string& file, std::string name) {
    if(name == "Auto") name += "("+file+")";
    unsigned int texHandle = 0;
    glGenTextures(1, &texHandle);
    unsigned int handle = MR::TextureLoader::GetInstance()->LoadGLTexture(file, texHandle);
    return dynamic_cast<MR::TextureResource*>(m->Create(name, handle));/*(new MR::TextureResource(m, name, file, texture_id, MR::ITexture::Target_Base2D))*/;
}

Resource* TextureManager::Create(const std::string& name, const std::string& source) {
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    TextureResource * t = new TextureResource(this, name, source, 0, MR::ITexture::Target_Base2D);
    this->_resources.push_back(t);
    return dynamic_cast<MR::Resource*>(t);
}

Resource* TextureManager::Create(const std::string& name, const unsigned int & gl_texture) {
    if(this->_debugMessages) MR::Log::LogString("TextureManager "+name+" (gl_texture) creating", MR_LOG_LEVEL_INFO);
    TextureResource * t = new TextureResource(this, name, "", gl_texture, MR::ITexture::Target_Base2D);
    t->ResetInfo();
    t->__set_loaded(true);
    this->_resources.push_back(t);
    return dynamic_cast<MR::Resource*>(t);
}

TextureManager::TextureManager() : ResourceManager() {
    unsigned int handle = 0;
    unsigned char pixel_white_data[4] {255,255,255,255};
    MR::Texture::CreateGLTexture(&pixel_white_data[0], 1, 1, MR::TextureLoader::LFO_RGBA, &handle, MR::TextureLoader::LFL_None);
    _white = new MR::Texture(handle, MR::ITexture::Target_Base2D);
}

TextureManager::~TextureManager() {
}

}
