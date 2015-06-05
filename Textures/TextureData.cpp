#include "TextureData.hpp"
#include <FreeImage.h>

#define MR_USE_FREE_IMAGE

namespace mr {

TextureDataPtr TextureData::FromMemory(mu::ArrayHandle<unsigned char> data, glm::uvec2 const& size, Texture::DataFormat const& dformat, Texture::DataType const& dtype) {
    TextureData* textureData = new TextureData();
    textureData->_dataFormat = dformat;
    textureData->_dataType = dtype;
    textureData->_size = size;
    textureData->_data = data;

    return TextureDataPtr(textureData);
}

#ifdef MR_USE_FREE_IMAGE

namespace {

/*
FI_ENUM(FREE_IMAGE_COLOR_TYPE) {
	FIC_MINISWHITE = 0,		//! min value is white
    FIC_MINISBLACK = 1,		//! min value is black
    FIC_RGB        = 2,		//! RGB color model
    FIC_PALETTE    = 3,		//! color map indexed
	FIC_RGBALPHA   = 4,		//! RGB color model with alpha channel
	FIC_CMYK       = 5		//! CMYK color model
};
*/
Texture::DataFormat MAP_FORMAT[6] = {
    Texture::DF_RED,
    Texture::DF_RED,
    Texture::DF_BGR,
    Texture::DF_COLOR_INDEX,
    Texture::DF_BGRA,
    Texture::DF_NONE
};

}

TextureDataPtr TextureData::FromFile(std::string const& file) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	fif = FreeImage_GetFileType(file.c_str(), 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file.c_str());
	if(fif == FIF_UNKNOWN)
		return nullptr;

    FIBITMAP* dib = 0;
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, file.c_str());
	if(!dib)
		return nullptr;
    FreeImage_FlipVertical(dib);

	BYTE* bits = FreeImage_GetBits(dib);
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	size_t dataSize = FreeImage_GetMemorySize(dib);

	//FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	FREE_IMAGE_COLOR_TYPE imageFormat = FreeImage_GetColorType(dib);

	if((bits == 0) || (width == 0) || (height == 0))
		return nullptr;

    TextureData* textureData = new TextureData();
    textureData->_data = mu::ArrayHandle<unsigned char>(bits, dataSize, true, true);
    textureData->_dataFormat = MAP_FORMAT[imageFormat];
    textureData->_dataType = Texture::DT_UNSIGNED_BYTE;
    textureData->_size = glm::uvec2(width, height);

	FreeImage_Unload(dib);

	return TextureDataPtr(textureData);
}

#endif

}
