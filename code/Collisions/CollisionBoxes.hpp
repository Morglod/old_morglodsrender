#pragma once

#ifndef _MR_COLLISION_BOXES_H_
#define _MR_COLLISION_BOXES_H_

#include "CollisionInterfaces.hpp"
#include "../Utils/Containers.hpp"

namespace mr {

/** Compute min and max points when making box **/

class Box : public IBox, public ICollidable {
public:
    inline glm::vec3 GetMin() override { return _min; }
    inline glm::vec3 GetMax() override { return _max; }
    inline glm::vec3 GetOffset() override { return _max - _min; }

    inline bool TestPoint(const glm::vec3& point) override {
        if(point.x < _min.x || point.y < _min.y || point.z < _min.z) return false;
        if(point.x > _max.x || point.y > _max.y || point.z > _max.z) return false;
        return true;
    }

    bool TestAABB(const glm::vec3& minPoint, const glm::vec3& size) override {
        if(TestPoint(minPoint)) return true;
        if(TestPoint(minPoint+size)) return true;
        if(TestPoint(minPoint+glm::vec3(size.x,0,0))) return true;
        if(TestPoint(minPoint+glm::vec3(0,size.y,0))) return true;
        if(TestPoint(minPoint+glm::vec3(0,0,size.z))) return true;
        if(TestPoint(minPoint+glm::vec3(size.x,0,size.z))) return true;
        if(TestPoint(minPoint+glm::vec3(size.x,size.y,0))) return true;
        if(TestPoint(minPoint+glm::vec3(0,size.y,size.z))) return true;
        return false;
    }

    bool TestFastSphere(const glm::vec3& center, const float& radius) override;
    bool TestFastFrustum(CollisionFrustumInfo const& info, glm::vec3 const& pos) override;
    bool TestRayFirstPoint(RayInfo const& ray, glm::vec3 * out_Point) override;
    bool TestRay(RayInfo const& ray, RayHitInfo<glm::vec3> * out_Points) override;

    Box(glm::vec3 const& p1, glm::vec3 const& p2);
    Box(mr::TStaticArray<glm::vec3> points);
protected:
    glm::vec3 _min, _max;
};

}

#endif // _MR_COLLISION_BOXES_H_
