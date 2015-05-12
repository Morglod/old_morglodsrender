#include "TextureData.hpp"
#include <FreeImage.h>

#define MR_USE_FREE_IMAGE

namespace mr {

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

class TextureData::Impl {
public:
    FIBITMAP* image = 0;
    FREE_IMAGE_FORMAT fileFormat = FIF_UNKNOWN;
    FREE_IMAGE_TYPE imageType = FIT_UNKNOWN;
    FREE_IMAGE_COLOR_TYPE imageFormat;

    Impl() = default;
    ~Impl() {
        if(image != 0) FreeImage_Unload(image);
        image = 0;
    }
};

unsigned char* TextureData::GetData() const {
    if(_impl == nullptr) return 0;
    return FreeImage_GetBits(_impl->image);
}

TextureDataPtr TextureData::FromFile(std::string const& file) {
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	BYTE* bits = 0; //image data
	unsigned int width = 0, height = 0;

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

	bits = FreeImage_GetBits(dib);
	width = FreeImage_GetWidth(dib);
	height = FreeImage_GetHeight(dib);

	FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	FREE_IMAGE_COLOR_TYPE imageFormat = FreeImage_GetColorType(dib);

	if((bits == 0) || (width == 0) || (height == 0))
		return nullptr;

    TextureData* textureData = new TextureData();
    TextureData::Impl* textureImpl = new TextureData::Impl();
    textureImpl->fileFormat = fif;
    textureImpl->image = dib;
    textureImpl->imageType = imageType;
    textureImpl->imageFormat = imageFormat;
    textureData->_dataType = Texture::DT_UNSIGNED_BYTE; //MAP_TYPE[imageType];
    textureData->_dataFormat = MAP_FORMAT[imageFormat];
    textureData->_size = glm::uvec2(width, height);
    textureData->_impl = std::shared_ptr<TextureData::Impl>(textureImpl);

	return TextureDataPtr(textureData);
}

#endif

}
