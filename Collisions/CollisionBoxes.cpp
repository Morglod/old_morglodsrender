#include "CollisionBoxes.hpp"

#define MR_MIN(x,y) ((x>y) ? y : x)
#define MR_MAX(x,y) ((x<y) ? y : x)

namespace MR {

bool Box::TestSphere(const glm::vec3& center, const float& radius) {
    if(TestPoint(center)) return true;
    const float xMaxPlane = _max.x, xMinPlane = _min.x, yMaxPlane = _max.y, yMinPlane = _min.y, zMaxPlane = _max.z, zMinPlane = _min.z;
    //Test dist for each side
    if( (center.x < xMaxPlane) && ((center.x+radius) >= xMaxPlane) ) return true;
    if( (center.x > xMinPlane) && ((center.x-radius) <= xMinPlane) ) return true;

    if( (center.y < yMaxPlane) && ((center.y+radius) >= yMaxPlane) ) return true;
    if( (center.y > yMinPlane) && ((center.y+radius) <= yMinPlane) ) return true;

    if( (center.z < zMaxPlane) && ((center.z+radius) >= zMaxPlane) ) return true;
    if( (center.z > zMinPlane) && ((center.z+radius) <= zMinPlane) ) return true;
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

}
