#pragma once

#include "TextureData.hpp"

namespace mr {

class Texture2D : public Texture {
    friend class TextureManager;
public:
    virtual bool SetData(TextureDataPtr const& data, Texture::StorageDataFormat const& sdf);
    virtual bool Storage(glm::uvec2 const& size, Texture::DataType const& dataType, Texture::DataFormat const& dataFormat, Texture::StorageDataFormat const& sdf);
protected:
    Texture2D() = default;
    bool Create(CreationParams const& params = Texture::CreationParams()) override;
};

}
