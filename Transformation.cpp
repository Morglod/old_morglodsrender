#include "Transformation.hpp"
#include <glm/gtx/transform.hpp>

namespace MR {

void Transform::Calc() {
    _mat = ( glm::translate(glm::mat4(1.0f), _pos) * ( glm::rotate(glm::mat4(1.0f), _rot.x, glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0f), _rot.y, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f), _rot.z, glm::vec3(0,0,1)) ) * glm::scale(glm::mat4(1.0f), _scale) );
    OnChanged.Call(this, _mat);
}

void Transform::SetAutoRecalc(const bool& state) {
    _autoReCalc = state;
    Calc();
}

void Transform::SetPos(const glm::vec3& p) {
    if(_pos!=p) {
        _pos = p;
        if(IsAutoRecalc()) Calc();
    }
}

void Transform::SetPos(glm::vec3* p) {
    _pos = *p;
    if(IsAutoRecalc()) Calc();
}

void Transform::SetRot(const glm::vec3& r) {
    if(_rot!=r) {
        _rot = r;
        if(IsAutoRecalc()) Calc();
    }
}

void Transform::SetRot(glm::vec3* r) {
    _rot = *r;
    if(IsAutoRecalc()) Calc();
}

void Transform::SetScale(const glm::vec3& s) {
    if(_scale!=s) {
        _scale = s;
        if(IsAutoRecalc()) Calc();
    }
}

void Transform::SetScale(glm::vec3* s) {
    _scale = *s;
    if(IsAutoRecalc()) Calc();
}

glm::vec2 Transform::NormalizedDirection(float angle) {
    float r = glm::radians(angle);
    return glm::vec2( glm::cos( r ), glm::sin( r ) );
}

glm::vec3 Transform::NormalizedDirection(float angleX, float angleY, float angleZ) {
    float rx = glm::radians(angleX);
    float ry = glm::radians(angleY);

    return glm::vec3(
        glm::cos( ry ) * glm::sin( rx ),
        glm::sin( ry ),
        glm::cos( ry ) * glm::cos( rx )
    );
}

}
