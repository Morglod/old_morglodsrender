#pragma once

#ifndef _MR_COLLISION_INTERFACES_H_
#define _MR_COLLISION_INTERFACES_H_

#include <glm/glm.hpp>

namespace MR {

class IBox {
public:
    virtual glm::vec3 GetMin() = 0;
    virtual glm::vec3 GetMax() = 0;
    virtual glm::vec3 GetOffset() = 0; // min + offset = max
};

struct CollisionFrustumInfo {
public:
    glm::vec3 direction;
    float nearPlane, farPlane, fov;
};

struct RayInfo {
    glm::vec3 point, dir;
};

template<typename pointT>
struct RayHitInfo {
    pointT* points;
    size_t num;
};

class ICollidable {
public:
    virtual bool TestPoint(const glm::vec3& point) = 0;
    virtual bool TestAABB(const glm::vec3& minPoint, const glm::vec3& size) = 0;
    virtual bool TestFastSphere(const glm::vec3& center, const float& radius) = 0; //sphere is cube (so test with cube)
    virtual bool TestSphere(const glm::vec3& center, const float& radius) = 0;
    virtual bool TestFrustum(CollisionFrustumInfo const& info, glm::vec3 const& pos) = 0;
    virtual bool TestRayFirstPoint(RayInfo const& ray, glm::vec3 * out_Point) = 0;
    virtual bool TestRay(RayInfo const& ray, RayHitInfo<glm::vec3> * out_Points) = 0;
};

}

#endif // _MR_COLLISION_INTERFACES_H_
