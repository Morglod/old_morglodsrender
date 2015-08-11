#pragma once

#include "Texture2D.hpp"
#include "../Buffers/BuffersInterfaces.hpp"

#include <mu/Singleton.hpp>
#include <mu/Containers.hpp>

#include <unordered_set>

namespace mr {

class TextureSettings;
class TextureUBO;

class TextureManager : public mu::StaticSingleton<TextureManager> {
public:
    Texture2D* CreateTexture2D(Texture::CreationParams const& params = Texture::CreationParams());
    Texture2D* CreateTexture2D(glm::uvec2 const& size, Texture::DataType const& dataType, Texture::DataFormat const& dataFormat, Texture::StorageDataFormat const& sdf, Texture::CreationParams const& params = Texture::CreationParams());
    Texture2D* LoadTexture2DFromFile(std::string const& file, Texture::CreationParams const& params = Texture::CreationParams());
    TextureUBO* MakeTextureUbo(mu::ArrayHandle<Texture*> const& textures, const bool fastChange = false);

    void DestroyAllTextures();
    void DestroyAllUBOs();

    void Delete(Texture*& texture);
    void Delete(TextureUBO*& textureUbo);

    TextureManager();
    virtual ~TextureManager();
private:
    void _RegisterTexture(Texture* tex);
    void _UnRegisterTexture(Texture* tex);
    void _RegisterUBO(TextureUBO* tex);
    void _UnRegisterUBO(TextureUBO* tex);
    std::unordered_set<Texture*> _textures;
    std::unordered_set<TextureUBO*> _ubos;
};

}
