#pragma once

#include <string>
#include "pre_glm.hpp"

namespace std {

inline std::string to_string(const glm::vec2& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y);
}

inline std::string to_string(const glm::ivec2& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y);
}

inline std::string to_string(const glm::vec3& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
}

inline std::string to_string(const glm::ivec3& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);
}

inline std::string to_string(const glm::vec4& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
}

inline std::string to_string(const glm::ivec4& v) {
    return std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ", " + std::to_string(v.w);
}

}
