#pragma once

#include "Texture2D.hpp"
#include "../Buffers/BuffersInterfaces.hpp"

#include <mu/Singleton.hpp>
#include <mu/Containers.hpp>

#include <unordered_set>

namespace mr {

class TextureSettings;

class TextureManager : public mu::StaticSingleton<TextureManager> {
public:
    Texture2D* CreateTexture2D(Texture::CreationParams const& params = Texture::CreationParams());
    Texture2D* LoadTexture2DFromFile(std::string const& file, Texture::CreationParams const& params = Texture::CreationParams());
    IGPUBuffer* MakeBindlessTexUbo(IGPUBuffer::Usage const& usage, mu::ArrayHandle<Texture*> const& textures);

    void DestroyAllTextures();

    TextureManager();
    virtual ~TextureManager();
private:
    void _RegisterTexture(Texture* tex);
    void _UnRegisterTexture(Texture* tex);
    std::unordered_set<Texture*> _textures;
};

}
