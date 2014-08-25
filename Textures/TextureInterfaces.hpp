#pragma once

#ifndef _MR_TEXTURE_INTERFACES_H_
#define _MR_TEXTURE_INTERFACES_H_

#include "../Utils/Events.hpp"
#include "../Types.hpp"
#include "../CoreObjects.hpp"

namespace MR {

class ITextureSettings : public Copyable<ITextureSettings*> {
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
    MR::EventListener<ITextureSettings*, const float&> OnLodBiasChanged;
    MR::EventListener<ITextureSettings*, float*> OnBorderColorChanged; //as param used pointer to _border_color
    MR::EventListener<ITextureSettings*, const MinFilter&> OnMinFilterChanged;
    MR::EventListener<ITextureSettings*, const MagFilter&> OnMagFilterChanged;
    MR::EventListener<ITextureSettings*, const int&> OnMinLodChanged;
    MR::EventListener<ITextureSettings*, const int&> OnMaxLodChanged;
    MR::EventListener<ITextureSettings*, const Wrap&> OnWrapSChanged;
    MR::EventListener<ITextureSettings*, const Wrap&> OnWrapRChanged;
    MR::EventListener<ITextureSettings*, const Wrap&> OnWrapTChanged;
    MR::EventListener<ITextureSettings*, const CompareMode&> OnCompareModeChanged;
    MR::EventListener<ITextureSettings*, const CompareFunc&> OnCompareFuncChanged;

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

    virtual unsigned int GetGLSampler() = 0;

    virtual ~ITextureSettings() {}
};

class ITexture {
public:
    enum InternalFormat {
        IFormat_ALPHA = 0x1906,
        IFormat_ALPHA4 = 0x803B,
        IFormat_ALPHA8 = 0x803C,
        IFormat_ALPHA12 = 0x803D,
        IFormat_ALPHA16 = 0x803E,
        IFormat_LUMINANCE = 0x1909,
        IFormat_LUMINANCE4 = 0x803F,
        IFormat_LUMINANCE8 = 0x8040,
        IFormat_LUMINANCE12 = 0x8041,
        IFormat_LUMINANCE16 = 0x8042,
        IFormat_LUMINANCE_ALPHA = 0x190A,
        IFormat_LUMINANCE4_ALPHA4 = 0x8043,
        IFormat_LUMINANCE6_ALPHA2 = 0x8044,
        IFormat_LUMINANCE8_ALPHA8 = 0x8045,
        IFormat_LUMINANCE12_ALPHA4 = 0x8046,
        IFormat_LUMINANCE12_ALPHA12 = 0x8047,
        IFormat_LUMINANCE16_ALPHA16 = 0x8048,
        IFormat_INTENSITY = 0x8049,
        IFormat_INTENSITY4 = 0x804A,
        IFormat_INTENSITY8 = 0x804B,
        IFormat_INTENSITY12 = 0x804C,
        IFormat_INTENSITY16 = 0x804D,
        IFormat_R3_G3_B2 = 0x2A10,
        IFormat_RGB = 0x1907,
        IFormat_RGB4 = 0x804F,
        IFormat_RGB5 = 0x8050,
        IFormat_RGB8 = 0x8051,
        IFormat_RGB10 = 0x8052,
        IFormat_RGB12 = 0x8053,
        IFormat_RGB16 = 0x8054,
        IFormat_RGBA = 0x1908,
        IFormat_RGBA2 = 0x8055,
        IFormat_RGBA4 = 0x8056,
        IFormat_RGB5_A1 = 0x8057,
        IFormat_RGBA8 = 0x8058,
        IFormat_RGB10_A2 = 0x8059,
        IFormat_RGBA12 = 0x805A,
        IFormat_RGBA16 = 0x805B
    };

    enum Format {
        Format_COLOR_INDEX = 0x1900,
        Format_RED = 0x1903,
        Format_GREEN = 0x1904,
        Format_BLUE = 0x1905,
        Format_ALPHA = 0x1906,
        Format_RGB = 0x1907,
        Format_RGBA = 0x1908,
        Format_BGR_EXT = 0x80E0,
        Format_BGRA_EXT = 0x80E1,
        Format_LUMINANCE = 0x1909,
        Format_LUMINANCE_ALPHA = 0x190A
    };

    enum Type {
        Type_UNSIGNED_BYTE = 0x1401,
        Type_BYTE = 0x1400,
        Type_UNSIGNED_SHORT = 0x1403,
        Type_SHORT = 0x1402,
        Type_UNSIGNED_INT = 0x1405,
        Type_INT = 0x1404,
        Type_FLOAT = 0x1406,
        Type_UNSIGNED_BYTE_3_3_2 = 0x8032,
        Type_UNSIGNED_BYTE_2_3_3_REV = 0x8362,
        Type_UNSIGNED_SHORT_5_6_5 = 0x8363,
        Type_UNSIGNED_SHORT_5_6_5_REV = 0x8364,
        Type_UNSIGNED_SHORT_4_4_4_4 = 0x8033,
        Type_UNSIGNED_SHORT_4_4_4_4_REV = 0x8365,
        Type_UNSIGNED_SHORT_5_5_5_1 = 0x8034,
        Type_UNSIGNED_SHORT_1_5_5_5_REV = 0x8366,
        Type_UNSIGNED_INT_8_8_8_8 = 0x8035,
        Type_UNSIGNED_INT_8_8_8_8_REV = 0x8367,
        Type_UNSIGNED_INT_10_10_10_2 = 0x8036,
        Type_UNSIGNED_INT_2_10_10_10_REV = 0x8368
    };

    enum Target {
        Target_Base1D = 0x0DE0,
        Target_Base2D = 0x0DE1,
        Target_Base3D = 0x806F,
        Target_Rectangle = 0x84F5,
        Target_Buffer = 0x8C2A,
        Target_CubeMap = 0x8513,
        Target_Array1D = 0x8C18,
        Target_Array2D = 0x8C1A,
        Target_CubeMapArray = 0x9009,
        Target_Multisample2D = 0x9100,
        Target_Multisample2DArray = 0x9102
    };

    enum CompressionMode {
        NoCompression = 0,
        DefaultCompression,
        S3TC,
        ETC2
    };

    MR::EventListener<ITexture*> OnInfoReset;

    /* mipMapLevel, internalFormat, width, height, format, type, data
     *  invokes from SetData method */
    MR::EventListener<ITexture*, const int&, const InternalFormat&, const int&, const int&, const Format&, const Type&, void*> OnDataChanged;

    /* mipMapLevel, xOffset, yOffset, width, height, format, type, data
     *  invokes form UpdateData method */
    MR::EventListener<ITexture*, const int&, const int&, const int&, const int&, const int&, const Format&, const Type&, void*> OnDataUpdated;

    MR::EventListener<ITexture*, ITextureSettings*> OnSettingsChanged;

    virtual ITextureSettings* GetSettings() = 0;
    virtual void SetSettings(ITextureSettings* ts) = 0;

    virtual unsigned short GetWidth() = 0;
    virtual unsigned short GetHeight() = 0;
    virtual unsigned short GetDepth() = 0;
    virtual InternalFormat GetInternalFormat() = 0;
    virtual unsigned int GetImageSize() = 0;
    virtual unsigned char GetRedBitsNum() = 0;
    virtual unsigned char GetGreenBitsNum() = 0;
    virtual unsigned char GetBlueBitsNum() = 0;
    virtual unsigned char GetDepthBitsNum() = 0;
    virtual unsigned char GetAlphaBitsNum() = 0;
    virtual bool IsCompressed() = 0;
    virtual unsigned int GetGPUHandle() = 0;
    virtual Target GetTarget() = 0;
    virtual void ResetInfo() = 0;
    virtual void GetData(const int& mipMapLevel, const Format& format, const Type& type, void* dstBuffer) = 0;
    virtual void SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data) = 0;
    virtual void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data) = 0;
    virtual ITexture::CompressionMode GetCompressionMode() = 0;

    virtual ~ITexture() {}
};

/*
class ITextureBinded {
public:
    virtual void UnBind() = 0;
    virtual ITexture* GetTexture() = 0;
    virtual int GetShaderInt() = 0;
    virtual bool NVBindless() = 0; //used nv extension, if true, get 'GetNVInt' otherwise use 'GetShaderInt'
    virtual uint64_t GetNVInt() = 0;
};

typedef std::shared_ptr<ITextureBinded> ITextureBindedPtr;*/

//TODO
class ITextureStream {
public:
    virtual bool Push(ITexture* srcTex, const size_t& srcOffset, const size_t& srcSize) = 0;
    virtual bool Pop(ITexture* dstTex, const size_t& dstOffset, const size_t& size) = 0;
};

}

#endif // _MR_TEXTURE_INTERFACES_H_
