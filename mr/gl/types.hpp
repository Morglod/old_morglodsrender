#pragma once

#include <unordered_map>
#include "../pre/glm.hpp"

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

#define _MR_IMPL_GL_TYPE(_type_, e_type_) \
template<> inline constexpr uint32_t GetTypeGL<_type_>(_type_* t) { return (uint32_t) UniformType:: e_type_ ; }


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

std::string VertexAttributeTypeString(uint32_t dataType, uint8_t components_num);

enum class UniformType : uint32_t {
    Float = 0x1406,
    Vec2 = 0x8B50,
    Vec3 = 0x8B51,
    Vec4 = 0x8B52,
    Int = 0x1404,
    IVec2 = 0x8B53,
    IVec3 = 0x8B54,
    IVec4 = 0x8B55,
    Bool = 0x8B56,
    BVec2 = 0x8B57,
    BVec3 = 0x8B58,
    BVec4 = 0x8B59,
    Mat2 = 0x8B5A,
    Mat2x3 = 0x8B65,
    Mat2x4 = 0x8B66,
    Mat3 = 0x8B5B,
    Mat3x2 = 0x8B67,
    Mat3x4 = 0x8B68,
    Mat4 = 0x8B5C,
    Mat4x2 = 0x8B69,
    Mat4x3 = 0x8B6A,
    Int64 = 0x140E,
    UInt64 = 0x140F,
    Int64_Vec2 = 0x8FE9,
    Int64_Vec3 = 0x8FEA,
    Int64_Vec4 = 0x8FEB,
    UInt64_Vec2 = 0x8FF5,
    UInt64_Vec3 = 0x8FF6,
    UInt64_Vec4 = 0x8FF7,
    Sampler1D = 0x8B5D,
    Sampler2D = 0x8B5E,
    Sampler3D = 0x8B5F
};

template<typename T> inline constexpr uint32_t GetTypeGL(T* t) { return 0; }
_MR_IMPL_GL_TYPE(float, Float);
_MR_IMPL_GL_TYPE(glm::vec2, Vec2);
_MR_IMPL_GL_TYPE(glm::vec3, Vec3);
_MR_IMPL_GL_TYPE(glm::vec4, Vec4);
_MR_IMPL_GL_TYPE(int32_t, Int);
_MR_IMPL_GL_TYPE(glm::ivec2, IVec2);
_MR_IMPL_GL_TYPE(glm::ivec3, IVec3);
_MR_IMPL_GL_TYPE(glm::ivec4, IVec4);
_MR_IMPL_GL_TYPE(bool, Bool);
_MR_IMPL_GL_TYPE(glm::bvec2, BVec2);
_MR_IMPL_GL_TYPE(glm::bvec3, BVec3);
_MR_IMPL_GL_TYPE(glm::bvec4, BVec4);
_MR_IMPL_GL_TYPE(glm::mat2, Mat2);
_MR_IMPL_GL_TYPE(glm::mat2x3, Mat2x3);
_MR_IMPL_GL_TYPE(glm::mat2x4, Mat2x4);
_MR_IMPL_GL_TYPE(glm::mat3, Mat3);
_MR_IMPL_GL_TYPE(glm::mat3x2, Mat3x2);
_MR_IMPL_GL_TYPE(glm::mat3x4, Mat3x4);
_MR_IMPL_GL_TYPE(glm::mat4, Mat4);
_MR_IMPL_GL_TYPE(glm::mat4x2, Mat4x2);
_MR_IMPL_GL_TYPE(glm::mat4x3, Mat4x3);
_MR_IMPL_GL_TYPE(int64_t, Int64);
_MR_IMPL_GL_TYPE(uint64_t, UInt64);
_MR_IMPL_GL_TYPE(glm::i64vec2, Int64_Vec2);
_MR_IMPL_GL_TYPE(glm::i64vec3, Int64_Vec3);
_MR_IMPL_GL_TYPE(glm::i64vec4, Int64_Vec4);
_MR_IMPL_GL_TYPE(glm::u64vec2, UInt64_Vec2);
_MR_IMPL_GL_TYPE(glm::u64vec3, UInt64_Vec3);
_MR_IMPL_GL_TYPE(glm::u64vec4, UInt64_Vec4);

}

#undef _MR_POS_DATA_TYPE_ENUM
#undef _MR_COLOR_DATA_TYPE_ENUM
#undef _MR_INDEX_DATA_TYPE_ENUM
#undef _MR_COLOR_DATA_TYPE_ENUM_UNIQ
