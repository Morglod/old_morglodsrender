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
    Texture::DF_RGB,
    Texture::DF_COLOR_INDEX,
    Texture::DF_RGBA,
    Texture::DF_NONE
};

/*
FI_ENUM(FREE_IMAGE_TYPE) {
	FIT_UNKNOWN = 0,	//! unknown type
	FIT_BITMAP  = 1,	//! standard image			: 1-, 4-, 8-, 16-, 24-, 32-bit
	FIT_UINT16	= 2,	//! array of unsigned short	: unsigned 16-bit
	FIT_INT16	= 3,	//! array of short			: signed 16-bit
	FIT_UINT32	= 4,	//! array of unsigned long	: unsigned 32-bit
	FIT_INT32	= 5,	//! array of long			: signed 32-bit
	FIT_FLOAT	= 6,	//! array of float			: 32-bit IEEE floating point
	FIT_DOUBLE	= 7,	//! array of double			: 64-bit IEEE floating point
	FIT_COMPLEX	= 8,	//! array of FICOMPLEX		: 2 x 64-bit IEEE floating point
	FIT_RGB16	= 9,	//! 48-bit RGB image			: 3 x 16-bit
	FIT_RGBA16	= 10,	//! 64-bit RGBA image		: 4 x 16-bit
	FIT_RGBF	= 11,	//! 96-bit RGB float image	: 3 x 32-bit IEEE floating point
	FIT_RGBAF	= 12	//! 128-bit RGBA float image	: 4 x 32-bit IEEE floating point
};
*/
Texture::DataType MAP_TYPE[13] = {
    Texture::DT_NONE,
    Texture::DT_INT,
    Texture::DT_UNSIGNED_SHORT,
    Texture::DT_SHORT,
    Texture::DT_UNSIGNED_INT,
    Texture::DT_INT,
    Texture::DT_FLOAT,
    Texture::DT_NONE,
    Texture::DT_NONE,
    Texture::DT_SHORT,
    Texture::DT_SHORT,
    Texture::DT_FLOAT,
    Texture::DT_FLOAT
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
    textureData->_dataType = MAP_TYPE[imageType];
    textureData->_dataFormat = MAP_FORMAT[imageFormat];
    textureData->_size = glm::uvec2(width, height);
    textureData->_impl = std::shared_ptr<TextureData::Impl>(textureImpl);

	return TextureDataPtr(textureData);
}

#endif

}
