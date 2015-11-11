#include "mr/camera.hpp"

namespace {

glm::vec3 _NormalizedDirection(float angleX, float angleY) {
    float rx = glm::radians(angleX);
    float ry = glm::radians(angleY);

    return glm::vec3(
        glm::cos( ry ) * glm::sin( rx ),
        glm::sin( ry ),
        glm::cos( ry ) * glm::cos( rx )
    );
}

}

namespace mr {

void PerspectiveCamera::CalcDir() {
    _dir_forward = _NormalizedDirection(_rot.x, _rot.y);
    _dir_up = _NormalizedDirection(_rot.x, _rot.y+90.0f);
    _dir_left = _NormalizedDirection(_rot.x+90.0f, 0.0f);
}

void PerspectiveCamera::CalcMat() {
    _mat = glm::lookAt(_pos, _pos + (_dir_forward * 2.0f), _dir_up);
}


}
