#pragma once

#include "Texture.hpp"
#include <mu/Containers.hpp>

#include <glm/glm.hpp>
#include <memory>

namespace mr {

class TextureData;
typedef std::shared_ptr<TextureData> TextureDataPtr;

class TextureData {
public:
    inline glm::uvec2 GetSize() const { return _size; }

    inline mu::ArrayHandle<unsigned char> GetData() const { return _data; }
    inline Texture::DataFormat GetDataFormat() const { return _dataFormat; }
    inline Texture::DataType GetDataType() const { return _dataType; }
    inline bool IsGood() const { return (_data.GetNum() != 0) && (_size != glm::uvec2(0,0)); }

    static TextureDataPtr FromMemory(mu::ArrayHandle<unsigned char> data, glm::uvec2 const& size, Texture::DataFormat const& dformat, Texture::DataType const& dtype);
    static TextureDataPtr FromFile(std::string const& file);

    ~TextureData() = default;
protected:
    TextureData() = default;

    mu::ArrayHandle<unsigned char> _data;
	glm::uvec2 _size = glm::uvec2(0,0);
	Texture::DataFormat _dataFormat = Texture::DF_NONE;
	Texture::DataType _dataType = Texture::DT_NONE;
};

}
