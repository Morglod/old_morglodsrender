#pragma once

#ifndef _MR_TEXTURE_INTERFACES_H_
#define _MR_TEXTURE_INTERFACES_H_

#include "../Types.hpp"
#include "../CoreObjects.hpp"

#include <Containers.hpp>

namespace mr {

class ITextureSettings : public IGPUObjectHandle {
public:
    enum MinFilter {
        MinFilter_NEAREST_MIPMAP_NEAREST = 0x2700,
        MinFilter_LINEAR_MIPMAP_NEAREST = 0x2701,
        MinFilter_NEAREST_MIPMAP_LINEAR = 0x2702,
        MinFilter_LINEAR_MIPMAP_LINEAR = 0x2703,
        MinFilter_NEAREST = 0x2600,
        MinFilter_LINEAR = 0x2601
    };

    enum MagFilter {
        MagFilter_NEAREST = 0x2600,
        MagFilter_LINEAR = 0x2601
    };

    enum Wrap {
        Wrap_CLAMP = 0x812F,
        Wrap_REPEAT = 0x2901,
        Wrap_MIRRORED_REPEAT = 0x8370,
        Wrap_DECAL = 0x2101
    };

    enum CompareMode {
        CompareMode_REF_TO_TEXTURE = 0x884E,
        CompareMode_NONE = 0
    };

    enum CompareFunc {
        CompareFunc_LEQUAL = 0x0203,
        CompareFunc_GEQUAL = 0x0206,
        CompareFunc_LESS = 0x0201,
        CompareFunc_GREATHER = 0x0204,
        CompareFunc_EQUAL = 0x0202,
        CompareFunc_NOTEQUAL = 0x0205,
        CompareFunc_ALWAYS = 0x0207,
        CompareFunc_NEVER = 0x0200
    };

    /* SENDER - TextureSettings pointer */
    mu::Event<ITextureSettings*, const float&> OnLodBiasChanged;
    mu::Event<ITextureSettings*, float*> OnBorderColorChanged; //as param used pointer to _border_color
    mu::Event<ITextureSettings*, const MinFilter&> OnMinFilterChanged;
    mu::Event<ITextureSettings*, const MagFilter&> OnMagFilterChanged;
    mu::Event<ITextureSettings*, const int&> OnMinLodChanged;
    mu::Event<ITextureSettings*, const int&> OnMaxLodChanged;
    mu::Event<ITextureSettings*, const Wrap&> OnWrapSChanged;
    mu::Event<ITextureSettings*, const Wrap&> OnWrapRChanged;
    mu::Event<ITextureSettings*, const Wrap&> OnWrapTChanged;
    mu::Event<ITextureSettings*, const CompareMode&> OnCompareModeChanged;
    mu::Event<ITextureSettings*, const CompareFunc&> OnCompareFuncChanged;

    virtual void SetLodBias(const float& v) = 0;
    virtual void SetBorderColor(float* rgba) = 0;
    virtual void SetBorderColor(const float& r, const float& g, const float& b, const float& a) = 0;
    virtual void SetMinFilter(const MinFilter& v) = 0;
    virtual void SetMagFilter(const MagFilter& v) = 0;
    virtual void SetMinLod(const int& v) = 0;
    virtual void SetMaxLod(const int& v) = 0;
    virtual void SetWrapS(const Wrap& v) = 0;
    virtual void SetWrapR(const Wrap& v) = 0;
    virtual void SetWrapT(const Wrap& v) = 0;
    virtual void SetCompareMode(const CompareMode& v) = 0;
    virtual void SetCompareFunc(const CompareFunc& v) = 0;

    virtual float GetLodBias() = 0;
    virtual float* GetBorderColor() = 0;
    virtual MinFilter GetMinFilter() = 0;
    virtual MagFilter GetMagFilter() = 0;
    virtual int GetMinLod() = 0;
    virtual int GetMaxLod() = 0;
    virtual Wrap GetWrapS() = 0;
    virtual Wrap GetWrapR() = 0;
    virtual Wrap GetWrapT() = 0;
    virtual CompareMode GetCompareMode() = 0;
    virtual CompareFunc GetCompareFunc() = 0;

    virtual void Create() = 0;

    /* GPUObjectHandle */
    //virtual unsigned int GetGPUHandle() = 0;

    /* ObjectHandle */
    //void Destroy();

    virtual ITextureSettings* Copy() = 0;

    virtual ~ITextureSettings() {}
};

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

class ITexture : public IGPUObjectHandle {
public:
    enum StorageDataFormat {
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

    enum DataFormat {
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

    enum DataType {
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

    enum Types {
        Base1D = 0,
        Base2D = 1,
        Base3D = 2
    };

    enum CompressionMode {
        NoCompression = 0,
        DefaultCompression,
        S3TC,
        ETC2
    };

    virtual ITextureSettings* GetSettings() = 0; //May be nullptr
    virtual void SetSettings(ITextureSettings* ts) = 0;

    virtual int GetMipMapLevelsNum() = 0;
    virtual TextureSizeInfo GetSizesInfo(int const& mipMapLevel) = 0;
    virtual TextureBitsInfo GetBitsInfo() = 0;
    virtual ITexture::StorageDataFormat GetDataStorageFormat() = 0;
    virtual bool IsCompressed() = 0;
    virtual ITexture::CompressionMode GetCompressionMode() = 0;

    virtual ITexture::Types GetType() = 0;

    virtual void Bind(unsigned short const& unit) = 0;
    virtual unsigned short Bind() = 0; //Binds at any free texture unit. returns 0 if failed.
    virtual ITexture* ReBind(unsigned short const& unit) = 0;

    virtual void Create(const ITexture::Types& type) = 0;

    virtual void GetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, unsigned int const& dstBufferSize,
                         void* dstBuffer) = 0; //dstBufferSize should be enought for data

    virtual void SetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, const ITexture::StorageDataFormat& sdFormat,
                         const int& width, const int& height, const int& depth,
                         void* data) = 0;

    virtual void UpdateData(const int& mipMapLevel,
                            const int& xOffset, const int& yOffset, const int& zOffset,
                            const int& width, const int& height, const int& depth,
                            const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                            void* data) = 0;

    virtual bool Complete(bool mipMaps) = 0;

    virtual void UpdateInfo() = 0;

    /* GPUObjectHandle */
    //virtual unsigned int GetGPUHandle();
    //virtual size_t GetGPUMem();

    /* ObjectHandle */
    //void Destroy();

    virtual ~ITexture() {}
};

}

#endif // _MR_TEXTURE_INTERFACES_H_
