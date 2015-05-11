#pragma once

#ifndef _MR_TEXTURE_OBJECTS_H_
#define _MR_TEXTURE_OBJECTS_H_

#include "TextureInterfaces.hpp"

namespace mr {

class Texture : public ITexture {
public:
    inline int GetMipMapLevelsNum() override { return _sizes.GetNum(); }
    inline TextureSizeInfo GetSizesInfo(int const& mipMapLevel) override { return _sizes.GetArray()[mipMapLevel]; }
    inline TextureBitsInfo GetBitsInfo() override { return _bits; }
    inline StorageDataFormat GetDataStorageFormat() override { return (StorageDataFormat)_data_storage_format; }
    inline bool IsCompressed() override { return _compressed; }
    inline ITexture::CompressionMode GetCompressionMode() { return _compression_mode; }

    inline Types GetType() override { return _texture_type; }

    inline TextureSettings* GetSettings() override { return _settings; }
    inline void SetSettings(TextureSettings* ts) override { _settings = ts; }

    inline size_t GetGPUMem() override { return _mem_size; }

    bool Create(const ITexture::Types& target) override;

    bool GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) override; //dstBufferSize should be enought for data

    bool SetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, const ITexture::StorageDataFormat& sdFormat,
                const int& width, const int& height, const int& depth,
                void* data) override;

    bool UpdateData(const int& mipMapLevel,
                    const int& xOffset, const int& yOffset, const int& zOffset,
                    const int& width, const int& height, const int& depth,
                    const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                    void* data) override;

    bool UpdateDataFromBuffer(const int& mipMapLevel,
                            const int& xOffset, const int& yOffset, const int& zOffset,
                            const int& width, const int& height, const int& depth,
                            const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                            class IGPUBuffer* buffer) override;

    bool Complete(bool mipMaps) override;
    bool UpdateInfo() override;

    void Destroy() override;

    inline bool GetResidentHandle(uint64_t& out) override {
        out = _residentHandle;
        return (_residentHandle != 0);
    }

    void MakeNonResident() override;

    Texture();
    virtual ~Texture();

protected:
    mu::ArrayHandle<TextureSizeInfo> _sizes;
    TextureBitsInfo _bits;
    size_t _mem_size;
    unsigned int _data_storage_format = 0;
    bool _compressed = false;
    TextureSettings* _settings = nullptr;
    ITexture::CompressionMode _compression_mode;
    Types _texture_type;
    bool _mipmaps = false;
    uint64_t _residentHandle = 0;
};

class TextureBindList : public ITextureBindList {
public:
    void Bind() override;
    inline void SetFirstUnit(unsigned short const& unit) override { _firstUnit = unit; }
    inline unsigned short GetFirstUnit() override { return _firstUnit; }
    inline ITexture* GetTexture(unsigned short const& index) override { return _textures.GetArray()[index]; }
    inline mu::ArrayRef<ITexture*> GetTextures() override { return _textures.ToRef(); }
    void SetTextures(mu::ArrayHandle<ITexture*> tex) override;
    void SetTexture(ITexture* tex, unsigned short const& index) override;
protected:
    unsigned short _firstUnit;
    mu::ArrayHandle<ITexture*> _textures;
    mu::ArrayHandle<unsigned int> _gpuHandles; // [_textures.GetNum()*2]{ texutre handles, sampler handles }
};

}

#endif // _MR_TEXTURE_OBJECTS_H_
