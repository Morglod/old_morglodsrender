#pragma once

#include "../CoreObjects.hpp"
#include <mu/Containers.hpp>

namespace mr {

class IBuffer;
class Texture;

/**
Textures resident-ptr UBO list.
**/
class TextureUBO : public IGPUObjectHandle {
    friend class TextureManager;
public:
    inline mu::ArrayRef<Texture*> GetTextures() const {
        return _textures.ToRef();
    }

    virtual bool Init(mu::ArrayHandle<Texture*> textures, const bool fastChange = false);
    virtual bool UpdateTextureHandle(size_t const& index);
    virtual bool IndexOf(Texture* texture, size_t& outIndex);

    void Destroy() override;
    size_t GetGPUMem() override;
    unsigned int GetGPUHandle() override;
    virtual IBuffer* GetBuffer() const;

    virtual ~TextureUBO();
protected:
    TextureUBO();

    IBuffer* _buffer = nullptr;
    mu::ArrayHandle<Texture*> _textures;
};

}
