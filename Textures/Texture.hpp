#pragma once

#include "../Types.hpp"
#include "TextureSettings.hpp"

#include <mu/Macro.hpp>
#include <mu/Containers.hpp>

namespace mr {

struct TextureSizeInfo {
public:
    unsigned short width, height, depth;

    inline unsigned long long CalcMaxVolume() { return ((unsigned long long)width * (unsigned long long)height * (unsigned long long)depth); }
    inline unsigned int CalcVolume() { return ((unsigned int)width * (unsigned int)height * (unsigned int)depth); }

    inline bool operator == (TextureSizeInfo const& tsi) const { return ((width == tsi.width) && (height == tsi.height) && (depth == tsi.depth)); }
    inline bool operator != (TextureSizeInfo const& tsi) const { return !(*this == tsi); }

    TextureSizeInfo();
    TextureSizeInfo(unsigned short const& w, unsigned short const& h, unsigned short const& d);
    TextureSizeInfo(glm::ivec3 const& s);
};

struct TextureBitsInfo {
public:
    unsigned char r, g, b, d, a; //red, green, blue, depth, alpha

    inline unsigned int CalcTotalBits() { return ((unsigned int)r + (unsigned int)g + (unsigned int)b + (unsigned int)d + (unsigned int)a); }

    inline bool operator == (TextureBitsInfo const& tbi) const { return ((r == tbi.r) && (g == tbi.g) && (b == tbi.b) && (d == tbi.d) && (a == tbi.a)); }
    inline bool operator != (TextureBitsInfo const& tbi) const { return !(*this == tbi); }

    TextureBitsInfo();
    TextureBitsInfo(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char d_, unsigned char a_);
};

//Base texutre class
class Texture : public IGPUObjectHandle {
    friend class TextureManager;
public:
    enum Type : unsigned int {
        //GL_TEXTURE_?D
        Type_None = 0,
        Base1D = 0x0DE0,
        Base2D = 0x0DE1,
        Base3D = 0x806F
    };

    enum CompressionMode {
        NoCompression = 0,
        DefaultCompression,
        S3TC,
        ETC2
    };

    enum StorageDataFormat : unsigned int {
        SDF_NONE = 0,
        SDF_ALPHA = 0x1906,
        SDF_ALPHA4 = 0x803B,
        SDF_ALPHA8 = 0x803C,
        SDF_ALPHA12 = 0x803D,
        SDF_ALPHA16 = 0x803E,
        SDF_LUMINANCE = 0x1909,
        SDF_LUMINANCE4 = 0x803F,
        SDF_LUMINANCE8 = 0x8040,
        SDF_LUMINANCE12 = 0x8041,
        SDF_LUMINANCE16 = 0x8042,
        SDF_LUMINANCE_ALPHA = 0x190A,
        SDF_LUMINANCE4_ALPHA4 = 0x8043,
        SDF_LUMINANCE6_ALPHA2 = 0x8044,
        SDF_LUMINANCE8_ALPHA8 = 0x8045,
        SDF_LUMINANCE12_ALPHA4 = 0x8046,
        SDF_LUMINANCE12_ALPHA12 = 0x8047,
        SDF_LUMINANCE16_ALPHA16 = 0x8048,
        SDF_INTENSITY = 0x8049,
        SDF_INTENSITY4 = 0x804A,
        SDF_INTENSITY8 = 0x804B,
        SDF_INTENSITY12 = 0x804C,
        SDF_INTENSITY16 = 0x804D,
        SDF_R3_G3_B2 = 0x2A10,
        SDF_RGB = 0x1907,
        SDF_RGB4 = 0x804F,
        SDF_RGB5 = 0x8050,
        SDF_RGB8 = 0x8051,
        SDF_RGB10 = 0x8052,
        SDF_RGB12 = 0x8053,
        SDF_RGB16 = 0x8054,
        SDF_RGBA = 0x1908,
        SDF_RGBA2 = 0x8055,
        SDF_RGBA4 = 0x8056,
        SDF_RGB5_A1 = 0x8057,
        SDF_RGBA8 = 0x8058,
        SDF_RGB10_A2 = 0x8059,
        SDF_RGBA12 = 0x805A,
        SDF_RGBA16 = 0x805B
    };

    enum DataFormat : unsigned int {
        DF_NONE = 0,
        DF_COLOR_INDEX = 0x1900,
        DF_RED = 0x1903,
        DF_GREEN = 0x1904,
        DF_BLUE = 0x1905,
        DF_ALPHA = 0x1906,
        DF_RGB = 0x1907,
        DF_RGBA = 0x1908,
        DF_BGR_EXT = 0x80E0,
        DF_BGRA_EXT = 0x80E1,
        DF_LUMINANCE = 0x1909,
        DF_LUMINANCE_ALPHA = 0x190A
    };

    enum DataType : unsigned int {
        DT_NONE = 0,
        DT_UNSIGNED_BYTE = 0x1401,
        DT_BYTE = 0x1400,
        DT_UNSIGNED_SHORT = 0x1403,
        DT_SHORT = 0x1402,
        DT_UNSIGNED_INT = 0x1405,
        DT_INT = 0x1404,
        DT_FLOAT = 0x1406,
        DT_UNSIGNED_BYTE_3_3_2 = 0x8032,
        DT_UNSIGNED_BYTE_2_3_3_REV = 0x8362,
        DT_UNSIGNED_SHORT_5_6_5 = 0x8363,
        DT_UNSIGNED_SHORT_5_6_5_REV = 0x8364,
        DT_UNSIGNED_SHORT_4_4_4_4 = 0x8033,
        DT_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
        DT_UNSIGNED_SHORT_5_5_5_1 = 0x8034,
        DT_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
        DT_UNSIGNED_INT_8_8_8_8 = 0x8035,
        DT_UNSIGNED_INT_8_8_8_8_REV = 0x8367,
        DT_UNSIGNED_INT_10_10_10_2 = 0x8036,
        DT_UNSIGNED_INT_2_10_10_10_REV = 0x8368
    };

    struct CreationParams {
        TextureSettings::Wrap   wrapS = TextureSettings::Wrap_REPEAT,
                                wrapT = TextureSettings::Wrap_REPEAT,
                                wrapR = TextureSettings::Wrap_REPEAT;
        TextureSettings::MinFilter minFilter = TextureSettings::MinFilter_LINEAR;
        TextureSettings::MagFilter magFilter = TextureSettings::MagFilter_LINEAR;
        CreationParams() = default;
    };

    void Destroy() override;
    inline size_t GetGPUMem() override { return _memSize; }

    inline virtual Texture::Type GetType() const { return _type; }
    inline virtual Texture::CompressionMode GetCompressionMode() const { return _compression; }
    inline virtual Texture::StorageDataFormat GetDataStorageFormat() const { return _storageDataFormat; }
    inline virtual Texture::DataFormat GetDataFormat() const { return _dataFormat; }
    inline virtual Texture::DataType GetDataType() const { return _dataType; }
    inline virtual bool IsCompressed() const { return _compression != NoCompression; }

    inline virtual int GetMipMapLevelsNum() const { return _sizes.GetNum(); }
    inline virtual TextureSizeInfo GetSizesInfo(int const& mipMapLevel) const { return _sizes.GetArray()[mipMapLevel]; }
    inline virtual TextureBitsInfo GetBitsInfo() const { return _bits; }
    virtual bool UpdateInfo();

    //dstBufferSize should be enought for data
    virtual bool GetData(int const& mipMapLevel,
                         Texture::DataFormat const& dformat, Texture::DataType const& dtype, unsigned int const& dstBufferSize,
                         void* dstBuffer);

    /**  If ARB_bindless_texture or NV_bindless_texture is supported (mr::gl::IsBindlessTextureSupported),
        Texture is locked by MakeResident(). **/
    virtual bool Complete(bool mipMaps);

    //ARB_bindless_texture or NV_bindless_texture should be supported & MakeResident() to set resident handle.
    inline virtual bool GetResidentHandle(uint64_t& out) const {
        if(_residentHandle == 0) return false;
        out = _residentHandle;
        return true;
    }

    //ARB_bindless_texture or NV_bindless_texture should be supported
    virtual bool MakeResident();

    ///Unlock texture, before massive changes (e.g. SetData)
    virtual void MakeNonResident();

    virtual ~Texture();

protected:
    Texture() = default;
    void _CalcMemSize();
    virtual bool Create(CreationParams const& params) = 0;
    inline virtual void SetType(Texture::Type const& type) { _type = type; }

private:
    Texture::Type _type = Type_None;
    Texture::CompressionMode _compression;
    Texture::StorageDataFormat _storageDataFormat = SDF_NONE;
    Texture::DataFormat _dataFormat = DF_NONE;
    Texture::DataType _dataType = DT_NONE;
    uint64_t _residentHandle = 0;
    size_t _memSize = 0;
    mu::ArrayHandle<TextureSizeInfo> _sizes;
    TextureBitsInfo _bits;
    bool _mipmaps = false; //mipmap generated?
};

}
