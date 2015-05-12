#pragma once

#include "Texture.hpp"

#include <glm/glm.hpp>
#include <memory>

namespace mr {

class TextureData;
typedef std::shared_ptr<TextureData> TextureDataPtr;

class TextureData {
public:
    inline glm::uvec2 GetSize() const { return _size; }
    unsigned char* GetData() const;
    inline Texture::DataFormat GetDataFormat() const { return _dataFormat; }
    inline Texture::DataType GetDataType() const { return _dataType; }
    inline bool IsGood() const { return (GetData() != 0) && (_impl != nullptr) && (_size != glm::uvec2(0,0)); }

    static TextureDataPtr FromFile(std::string const& file);
    ~TextureData() = default;
protected:
    TextureData() = default;

    class Impl;
    std::shared_ptr<Impl> _impl = nullptr;
	glm::uvec2 _size = glm::uvec2(0,0);
	Texture::DataFormat _dataFormat = Texture::DF_NONE;
	Texture::DataType _dataType = Texture::DT_NONE;
};

}
