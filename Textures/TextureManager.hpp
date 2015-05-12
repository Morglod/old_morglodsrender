#pragma once

#include "Texture2D.hpp"

#include <mu/Singleton.hpp>

#include <unordered_set>

namespace mr {

class IGPUBuffer;
class TextureSettings;

class TextureManager : public mu::StaticSingleton<TextureManager> {
public:
    Texture2D* CreateTexture2D(Texture::CreationParams const& params = Texture::CreationParams());
    Texture2D* LoadTexture2DFromFile(std::string const& file, Texture::CreationParams const& params = Texture::CreationParams());

    void DestroyAllTextures();

    TextureManager();
    virtual ~TextureManager();
private:
    void _RegisterTexture(Texture* tex);
    void _UnRegisterTexture(Texture* tex);
    std::unordered_set<Texture*> _textures;
};

}
