#pragma once

#ifndef _MR_TEXTURE_OBJECTS_H_
#define _MR_TEXTURE_OBJECTS_H_

#include "TextureInterfaces.hpp"

namespace mr {

class Texture : public ITexture {
public:
    inline int GetMipMapLevelsNum() override { return _sizes.GetNum(); }
    inline TextureSizeInfo GetSizesInfo(int const& mipMapLevel) override { return _sizes.At(mipMapLevel); }
    inline TextureBitsInfo GetBitsInfo() override { return _bits; }
    inline StorageDataFormat GetDataStorageFormat() override { return (StorageDataFormat)_data_storage_format; }
    inline bool IsCompressed() override { return _compressed; }
    inline ITexture::CompressionMode GetCompressionMode() { return _compression_mode; }

    inline Types GetType() override { return _texture_type; }

    inline ITextureSettings* GetSettings() override { return _settings; }
    inline void SetSettings(ITextureSettings* ts) override { _settings = ts; }

    inline size_t GetGPUMem() override { return _mem_size; }

    void Bind(unsigned short const& unit) override;
    unsigned short Bind() override;
    ITexture* ReBind(unsigned short const& unit) override;

    void Create(const ITexture::Types& target) override;

    void GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) override; //dstBufferSize should be enought for data

    void SetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, const ITexture::StorageDataFormat& sdFormat,
                const int& width, const int& height, const int& depth,
                void* data) override;

    void UpdateData(const int& mipMapLevel,
                    const int& xOffset, const int& yOffset, const int& zOffset,
                    const int& width, const int& height, const int& depth,
                    const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                    void* data) override;

    bool Complete(bool mipMaps) override;
    void UpdateInfo() override;

    void Destroy() override;

    Texture();
    virtual ~Texture();

    static ITexture* FromFile(std::string const& path);
    static ITexture* CreateMipmapChecker();

protected:
    mr::TStaticArray<TextureSizeInfo> _sizes;
    TextureBitsInfo _bits;
    size_t _mem_size;
    unsigned int _data_storage_format = 0;
    bool _compressed = false;
    ITextureSettings* _settings = nullptr;
    ITexture::CompressionMode _compression_mode;
    Types _texture_type;
    bool _mipmaps = false;
};

ITexture* TextureGetBinded(const unsigned short& unit);
void TextureUnBind(const unsigned short& unit, const bool& fast);
unsigned short TextureFreeUnit();

void DestroyAllTextures();

}

#endif // _MR_TEXTURE_OBJECTS_H_
