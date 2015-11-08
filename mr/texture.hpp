#pragma once

#include "build.hpp"
#include "memory.hpp"
#include "pre/glm.hpp"

#include <memory>
#include <string>

namespace mr {

typedef std::shared_ptr<class TextureData> TextureDataPtr;
typedef std::shared_ptr<class Texture2D> Texture2DPtr;

enum class TextureType : uint32_t {
    Base1D = 0x0DE0,
    Base2D = 0x0DE1,
    Base3D = 0x806F
};

enum class TextureStorageFormat : uint32_t {
    Alpha = 0x1906,
    Alpha4 = 0x803b,
    Alpha8 = 0x803c,
    Alpha12 = 0x803d,
    Alpha16 = 0x803e,
    Luminance = 0x1909,
    Luminance4 = 0x803f,
    Luminance8 = 0x8040,
    Luminance12 = 0x8041,
    Luminance16 = 0x8042,
    LuminanceAlpha = 0x190a,
    Luminance4Alpha4 = 0x8043,
    Luminance6Alpha2 = 0x8044,
    Luminance8Alpha8 = 0x8045,
    Luminance12Alpha4 = 0x8046,
    Luminance12Alpha12 = 0x8047,
    Luminance16Alpha16 = 0x8048,
    Intensity = 0x8049,
    Intensity4 = 0x804a,
    Intensity8 = 0x804b,
    Intensity12 = 0x804c,
    Intensity16 = 0x804d,
    R3G3B2 = 0x2a10,
    RGB = 0x1907,
    RGB4 = 0x804f,
    RGB5 = 0x8050,
    RGB8 = 0x8051,
    RGB10 = 0x8052,
    RGB12 = 0x8053,
    RGB16 = 0x8054,
    RGBA = 0x1908,
    RGBA2 = 0x8055,
    RGBA4 = 0x8056,
    RGB5A1 = 0x8057,
    RGBA8 = 0x8058,
    RGB10A2 = 0x8059,
    RGBA12 = 0x805a,
    RGBA16 = 0x805b,

    /** render buffer **/
    Depth24Stencil8 = 0x88f0,
    //rgba4,
    DepthComponent = 0x1902,
    DepthComponent16 = 0x81a5,
    DepthComponent24 = 0x81a6,
    DepthComponent32 = 0x81a7,
    DepthComponent32F = 0x8cac /// gl_arb_depth_buffer_float
};

enum class TextureDataFormat : uint32_t {
    ColorIndex = 0x1900,
    Red = 0x1903,
    Green = 0x1904,
    Blue = 0x1905,
    Alpha = 0x1906,
    RGB = 0x1907,
    RGBA = 0x1908,
    BGR_EXT = 0x80e0,
    BGRA_EXT = 0x80e1,
    Luminance = 0x1909,
    LuminanceAlpha = 0x190a,
    BGR = 0x80e0,
    BGRA = 0x80e1
};

enum class TextureDataType : uint32_t {
    UByte = 0x1401,
    Byte = 0x1400,
    UShort = 0x1403,
    Short = 0x1402,
    UInt = 0x1405,
    Int = 0x1404,
    Float = 0x1406,
    UByte_3_3_2 = 0x8032,
    UByte_2_3_3_rev = 0x8362,
    UShort_5_6_5 = 0x8363,
    UShort_5_6_5_rev = 0x8364,
    UShort_4_4_4_4 = 0x8033,
    UShort_4_4_4_4_rev = 0x8365,
    UShort_5_5_5_1 = 0x8034,
    UShort_1_5_5_5_rev = 0x8366,
    UInt_8_8_8_8 = 0x8035,
    UInt_8_8_8_8_rev = 0x8367,
    UInt_10_10_10_2 = 0x8036,
    UInt_2_10_10_10_rev = 0x8368
};

enum class TextureWrap : uint32_t {
    Clamp = 0x812F,
    Repeat = 0x2901,
    MirroredRepeat = 0x8370,
    Decal = 0x2101
};

enum class TextureMinFilter : uint32_t {
    Nearest = 0x2600,
    Linear = 0x2601,
    NearestMipmapNearest = 0x2700,
    LinearMipmapNearest = 0x2701,
    NearestMipmapLinear = 0x2702,
    LinearMipmapLinear = 0x2703
};

enum class TextureMagFilter : uint32_t {
    Nearest = 0x2600,
    Linear = 0x2601
};

struct TextureParams {
    TextureMinFilter minFilter = TextureMinFilter::Nearest;
    TextureMagFilter magFilter = TextureMagFilter::Nearest;
    TextureWrap      wrapS = TextureWrap::Clamp,
                     wrapR = TextureWrap::Clamp,
                     wrapT = TextureWrap::Clamp;
    TextureStorageFormat storageFormat = TextureStorageFormat::RGB8;
    uint32_t levels = 9;
    glm::uvec3 size = glm::uvec3(1,0,0);
};

class MR_API TextureData final {
public:
    inline MemoryPtr GetData() const;
    inline TextureDataFormat GetFormat() const;
    inline TextureDataType GetType() const;
    inline glm::uvec3 GetSize() const;
    inline bool IsGood() const;
    inline TextureType GetTextureType() const;

    static TextureDataPtr FromFile(std::string const& file);
private:
    TextureData() = default;

    MemoryPtr _data;
    TextureDataFormat _dataFormat;
    TextureDataType _dataType;
    glm::uvec3 _size;
};

class MR_API Texture2D final {
public:
    inline uint32_t GetId() const;
    static Texture2DPtr Create(TextureParams const& params = TextureParams());
    bool Storage();
    bool WriteImage(TextureDataPtr const& data, uint32_t level = 0);
    bool WriteSubImage(TextureDataPtr const& data, uint32_t level = 0, glm::ivec2 const& offset = glm::ivec2(0,0));
    bool BuildMipmaps();
    void Destroy();
    bool MakeResident();
    bool MakeNonResident();
    inline uint64_t GetResidentHandle() const;

    virtual ~Texture2D();
private:
    uint32_t _id;
    TextureParams _params;
    uint64_t _residentHandle = 0;
};

inline MemoryPtr TextureData::GetData() const {
    return _data;
}

inline TextureDataFormat TextureData::GetFormat() const {
    return _dataFormat;
}

inline TextureDataType TextureData::GetType() const {
    return _dataType;
}

inline glm::uvec3 TextureData::GetSize() const {
    return _size;
}

inline bool TextureData::IsGood() const {
    return (_data->GetSize() != 0) && (_size != glm::uvec3(0,0,0));
}

inline TextureType TextureData::GetTextureType() const {
    if(_size.z <= 1) return (_size.y <= 1) ? TextureType::Base1D : TextureType::Base2D;
    else return TextureType::Base3D;
}

inline uint32_t Texture2D::GetId() const {
    return _id;
}

inline uint64_t Texture2D::GetResidentHandle() const {
    return _residentHandle;
}

}
