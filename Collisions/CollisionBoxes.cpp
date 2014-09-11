#include "CollisionBoxes.hpp"
#include "../Utils/Log.hpp"

#define MR_MIN(x,y) ((x>y) ? y : x)
#define MR_MAX(x,y) ((x<y) ? y : x)

namespace MR {

bool Box::TestFastSphere(const glm::vec3& center, const float& radius) {
    if(TestPoint(center)) return true;
    glm::vec3 r3(radius, radius, radius);
    return TestAABB(center - r3, r3);
}

bool Box::TestFastFrustum(CollisionFrustumInfo const& info, glm::vec3 const& pos) {
    MR::Log::LogString("Box::TestFastFrustum not yet implemented.");
    return false;
}

bool Box::TestRayFirstPoint(RayInfo const& ray, glm::vec3 * out_Point) {
    MR::Log::LogString("Box::TestRayFirstPoint not yet implemented.");
    return false;
}

bool Box::TestRay(RayInfo const& ray, RayHitInfo<glm::vec3> * out_Points) {
    MR::Log::LogString("Box::TestRay not yet implemented.");
    return false;
}

Box::Box(glm::vec3 const& p1, glm::vec3 const& p2) {
    _min = glm::vec3(
                     MR_MIN(p1.x, p2.x),
                     MR_MIN(p1.y, p2.y),
                     MR_MIN(p1.z, p2.z)
                     );
    _max = glm::vec3(
                     MR_MAX(p1.x, p2.x),
                     MR_MAX(p1.y, p2.y),
                     MR_MAX(p1.z, p2.z)
                     );
}

Box::Box(TStaticArray<glm::vec3> points) {
    glm::vec3* p = points.GetRaw();
    for(size_t i = 0; i < points.GetNum(); ++i){
        _min.x = MR_MIN(_min.x, p[i].x);
        _min.y = MR_MIN(_min.y, p[i].y);
        _min.z = MR_MIN(_min.z, p[i].z);

        _max.x = MR_MAX(_max.x, p[i].x);
        _max.y = MR_MAX(_max.y, p[i].y);
        _max.z = MR_MAX(_max.z, p[i].z);
    }
}

}
