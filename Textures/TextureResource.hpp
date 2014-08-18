#pragma once

#ifndef _MR_TEXTURE_RESOURCE_H_
#define _MR_TEXTURE_RESOURCE_H_

#include "../Resources/ResourceManager.hpp"
#include "TextureObjects.hpp"

namespace MR {

class TextureManager;

class TextureResource : public Texture, public virtual Resource {
public:
    TextureResource(TextureManager* manager, const std::string& name, const std::string& source, const unsigned int& handle, const MR::ITexture::Target& target);
    virtual ~TextureResource();

    static TextureResource* FromFile(MR::TextureManager* m, const std::string& file, std::string name = "Auto");
protected:
    bool _Loading() override;
    void _UnLoading() override;

    unsigned char* _async_data = 0;
    int _async_iform = 0;
    int _async_form = 0;
};

class TextureManager : public virtual MR::ResourceManager, public Singleton<TextureManager> {
    friend class TextureResource;
public:
    virtual Resource* Create(const std::string& name, const std::string& source);
    virtual Resource* Create(const std::string& name, const unsigned int & gl_texture);

    inline TextureResource* NeedTexture(const std::string& source) {
        return dynamic_cast<TextureResource*>(Need(source));
    }

    inline ITexture* WhiteTexture() {
        return dynamic_cast<ITexture*>(_white);
    }

    inline void SetCompressionMode(const ITexture::CompressionMode& cm) {
        _defaultCompressionMode = cm;
    }
    inline ITexture::CompressionMode GetCompressionMode() {
        return _defaultCompressionMode;
    }

    TextureManager();
    virtual ~TextureManager();
protected:
    MR::Texture* _white;
    ITexture::CompressionMode _defaultCompressionMode;
};

}

#endif // _MR_TEXTURE_RESOURCE_H_
