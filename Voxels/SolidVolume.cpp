#include "SolidVolume.hpp"

namespace MR {

SolidVolume::UIndex3::UIndex3() : x(0), y(0), z(0) {}
SolidVolume::UIndex3::UIndex3(const unsigned char& _x, const unsigned char& _y, const unsigned char& _z) : x(_x), y(_y), z(_z) {}
SolidVolume::UIndex3::UIndex3(const glm::uvec3& v) : x((unsigned char)v.x), y((unsigned char)v.y), z((unsigned char)v.z) {}
SolidVolume::UIndex3::UIndex3(const glm::vec3& v) : x((unsigned char)v.x), y((unsigned char)v.y), z((unsigned char)v.z) {}

SolidVolume::SolidVolume() {
}

SolidVolume::~SolidVolume() {
}

}
