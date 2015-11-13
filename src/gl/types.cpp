#include "mr/gl/types.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"

namespace mr {

std::string VertexAttributeTypeString(uint32_t dataType, uint8_t components_num) {
    const std::unordered_map<uint32_t, std::string> spec_name_map = {
        {GL_BOOL_VEC2, "bvec2"},
        {GL_BOOL_VEC3, "bvec3"},
        {GL_BOOL_VEC4, "bvec4"},
        {GL_INT_VEC2, "ivec2"},
        {GL_INT_VEC3, "ivec3"},
        {GL_INT_VEC4, "ivec4"},
        {GL_UNSIGNED_INT_VEC2, "uvec2"},
        {GL_UNSIGNED_INT_VEC3, "uvec3"},
        {GL_UNSIGNED_INT_VEC4, "uvec4"},
        {GL_FLOAT_VEC2, "vec2"},
        {GL_FLOAT_VEC3, "vec3"},
        {GL_FLOAT_VEC4, "vec4"},
        {GL_BOOL_VEC2, "bvec2"},
        {GL_BOOL_VEC2, "bvec2"},
        {GL_BOOL_VEC2, "bvec2"},
        {GL_BOOL_VEC2, "bvec2"},
        {GL_FLOAT_MAT2, "mat2"},
        {GL_FLOAT_MAT2x3, "mat2x3"},
        {GL_FLOAT_MAT2x4, "mat2x4"},
        {GL_FLOAT_MAT3x2, "mat3x2"},
        {GL_FLOAT_MAT3x4, "mat3x4"},
        {GL_FLOAT_MAT4, "mat4"},
        {GL_FLOAT_MAT4x2, "mat4x2"},
        {GL_FLOAT_MAT4x3, "mat4x3"},
    };
    auto it = spec_name_map.find(dataType);
    if(it != spec_name_map.end()) return it->second;
    if(components_num <= 1) {
        const std::unordered_map<uint32_t, std::string> name_map = {
            {GL_BOOL, "bool"},
            {GL_INT, "int"},
            {GL_UNSIGNED_INT, "uint"},
            {GL_FLOAT, "float"},
            {GL_DOUBLE, "double"}
        };
        auto it2 = name_map.find(dataType);
        if(it2 != name_map.end()) return it2->second;
    } else {
        const std::unordered_map<uint32_t, std::string> name_map = {
            {GL_BOOL, "b"},
            {GL_INT, "i"},
            {GL_UNSIGNED_INT, "u"},
            {GL_FLOAT, ""},
            {GL_DOUBLE, "d"}
        };
        auto it2 = name_map.find(dataType);
        if(it2 != name_map.end()) return it2->second + "vec" + std::to_string(components_num);
        return "vec" + std::to_string(components_num);
    }

    MR_LOG_ERROR(VertexAttributeTypeString, "failed get type");
    return "";
}

}
