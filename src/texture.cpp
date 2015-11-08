#include "mr/texture.hpp"
#include "mr/log.hpp"
#include "src/mp.hpp"
#include "mr/pre/glew.hpp"

#include <FreeImage.h>

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
mr::TextureDataFormat MAP_FORMAT[5] = {
    mr::TextureDataFormat::Red,
    mr::TextureDataFormat::Red,
    mr::TextureDataFormat::BGR,
    mr::TextureDataFormat::ColorIndex,
    mr::TextureDataFormat::BGRA
    // CMYK skipped by assertion
};

}

namespace mr {

TextureDataPtr TextureData::FromFile(std::string const& file) {
    MP_ScopeSample(TextureData::FromFile);

    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	fif = FreeImage_GetFileType(file.c_str(), 0);
	if(fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilename(file.c_str());
	if(fif == FIF_UNKNOWN) {
		MR_LOG_ERROR(TextureData::FromFile, "failed recognize file type. \""+file+"\"");
		return nullptr;
	}

    FIBITMAP* dib = 0;
	if(FreeImage_FIFSupportsReading(fif))
		dib = FreeImage_Load(fif, file.c_str());
    else {
		MR_LOG_ERROR(TextureData::FromFile, "file format not supported. \""+file+"\"");
		return nullptr;
    }

	if(!dib) {
		MR_LOG_ERROR(TextureData::FromFile, "FreeImage_Load failed. \""+file+"\"");
		return nullptr;
	}

    //FreeImage_FlipVertical(dib);

	BYTE* bits = FreeImage_GetBits(dib);
	unsigned int width = FreeImage_GetWidth(dib);
	unsigned int height = FreeImage_GetHeight(dib);
	size_t dataSize = FreeImage_GetMemorySize(dib);

	//FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(dib);
	FREE_IMAGE_COLOR_TYPE imageFormat = FreeImage_GetColorType(dib);

	if(imageFormat == FIC_CMYK) {
		MR_LOG_ERROR(TextureData::FromFile, "CMYK color model not supported. \""+file+"\"");
        FreeImage_Unload(dib);
		return nullptr;
	}

	if((bits == 0) || (width == 0) || (height == 0)) {
		MR_LOG_ERROR(TextureData::FromFile, "corrupted data (bits/width/height is zero). \""+file+"\"");
        FreeImage_Unload(dib);
		return nullptr;
	}

	auto mem = mr::Memory::Copy(bits, dataSize);
	FreeImage_Unload(dib);

    auto textureData = TextureDataPtr(new TextureData());
    textureData->_data = mem;
    textureData->_dataFormat = MAP_FORMAT[imageFormat];
    textureData->_dataType = TextureDataType::UByte;
    textureData->_size = glm::uvec3(width, height, 1);

	return textureData;
}

Texture2DPtr Texture2D::Create(TextureParams const& params) {
    uint32_t handle;
    glCreateTextures(GL_TEXTURE_2D, 1, &handle);

    glTextureParameteri(handle, GL_TEXTURE_WRAP_S, (int32_t)params.wrapS);
    glTextureParameteri(handle, GL_TEXTURE_WRAP_T, (int32_t)params.wrapT);
    //glTextureParameteri(handle, GL_TEXTURE_WRAP_R, params.wrapR);
    glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, (int32_t)params.minFilter);
    glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, (int32_t)params.magFilter);

    Texture2DPtr tex = Texture2DPtr(new Texture2D());
    tex->_id = handle;
    tex->_params = params;

    return tex;
}

bool Texture2D::Storage() {
    glTextureStorage2D(_id, _params.levels, (uint32_t)_params.storageFormat, _params.size.x, _params.size.y);
    return true;
}

bool Texture2D::WriteImage(TextureDataPtr const& data, uint32_t level) {
    if(!data->IsGood()) {
        MR_LOG_ERROR(Texture2D::WriteImage, "data is bad");
        return false;
    }

    const auto dataSize = data->GetSize();

    glTextureImage2DEXT(_id,
                        GL_TEXTURE_2D,
                        (int32_t)level,
                        (uint32_t)_params.storageFormat,
                        dataSize.x, dataSize.y,
                        0,
                        (uint32_t)data->GetFormat(),
                        (uint32_t)data->GetType(),
                        data->GetData()->GetPtr());

    return true;
}

bool Texture2D::WriteSubImage(TextureDataPtr const& data, uint32_t level, glm::ivec2 const& offset) {
    if(!data->IsGood()) {
        MR_LOG_ERROR(Texture2D::WriteSubImage, "data is bad");
        return false;
    }

    const auto dataSize = data->GetSize();

    glTextureSubImage2D(_id,
                        (int32_t)level,
                        offset.x, offset.y,
                        dataSize.x, dataSize.y,
                        (uint32_t)data->GetFormat(),
                        (uint32_t)data->GetType(),
                        data->GetData()->GetPtr());

    return true;
}

bool Texture2D::BuildMipmaps() {
    glGenerateTextureMipmap(_id);
    return true;
}

void Texture2D::Destroy() {
    if(_id == 0) return;
    glDeleteTextures(1, &_id);
    _id = 0;
}

bool Texture2D::MakeResident() {
    _residentHandle = glGetTextureHandleARB(_id);
    glMakeTextureHandleResidentARB(_residentHandle);
    return true;
}

bool Texture2D::MakeNonResident() {
    if(_residentHandle == 0) return true;
    glMakeTextureHandleNonResidentARB(_residentHandle);
    _residentHandle = 0;
    return true;
}

Texture2D::~Texture2D() {
    Destroy();
}

}
