#pragma once

#include <unordered_map>

namespace mr {

enum class PosDataType : uint32_t {
    Float = 0x1406,
    HalfFloat = 0x140B,
    Int = 0x1404,
    Short = 0x1402
};

enum class ColorDataType : uint32_t {
    UByte = 0x1401,
    UShort = 0x1403,
    UInt = 0x1405,
    HalfFloat = 0x140B,
    Float = 0x1406
};

enum class IndexType : uint32_t {
    UShort = 0x1403,
    UInt = 0x1405
};

inline constexpr uint32_t sizeof_gl(uint32_t const& gl_dt) {
    switch(gl_dt) {
    case 0x1406: // Float
        return sizeof(float);
        break;
    case 0x140B: // HalfFloat
        return sizeof(float)/2;
        break;
    case 0x1404: // Int
        return sizeof(int32_t);
        break;
    case 0x1402: // Short
        return sizeof(int16_t);
        break;
    case 0x1401: // UByte
        return sizeof(uint8_t);
        break;
    case 0x1403: // UShort
        return sizeof(uint16_t);
        break;
    case 0x1405: // UInt
        return sizeof(uint32_t);
        break;
    default:
        return 0;
    }
    return 0;
}

}
