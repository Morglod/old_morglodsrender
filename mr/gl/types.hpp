#pragma once

#include <unordered_map>

#define _MR_POS_DATA_TYPE_ENUM \
    Float = 0x1406, \
    HalfFloat = 0x140B, \
    Int = 0x1404, \
    Short = 0x1402

#define _MR_COLOR_DATA_TYPE_ENUM_UNIQ \
    UByte = 0x1401, \
    UShort = 0x1403, \
    UInt = 0x1405

#define _MR_COLOR_DATA_TYPE_ENUM \
    _MR_COLOR_DATA_TYPE_ENUM_UNIQ, \
    HalfFloat = 0x140B, \
    Float = 0x1406

#define _MR_INDEX_DATA_TYPE_ENUM \
    UShort = 0x1403, \
    UInt = 0x1405

namespace mr {

enum class PosDataType : uint32_t {
    _MR_POS_DATA_TYPE_ENUM
};

enum class ColorDataType : uint32_t {
    _MR_COLOR_DATA_TYPE_ENUM
};

enum class IndexDataType : uint32_t {
    _MR_INDEX_DATA_TYPE_ENUM
};

enum class DataType : uint32_t {
    _MR_POS_DATA_TYPE_ENUM,
    _MR_COLOR_DATA_TYPE_ENUM_UNIQ,
    _MR_INDEX_DATA_TYPE_ENUM_UNIQ
};

inline constexpr uint32_t sizeof_gl(uint32_t const& gl_dt) {
    switch((uint32_t)gl_dt) {
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

#undef _MR_POS_DATA_TYPE_ENUM
#undef _MR_COLOR_DATA_TYPE_ENUM
#undef _MR_INDEX_DATA_TYPE_ENUM
#undef _MR_COLOR_DATA_TYPE_ENUM_UNIQ
