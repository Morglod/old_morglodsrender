#include "Transformation.hpp"

namespace MR {

void Transform::Calc() {
#ifdef MR_USE_OPENMP
    glm::mat4 tm, rmx, rmy, rmz, sm;
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            tm = glm::translate(glm::mat4(1.0f), _pos);
        }
        #pragma omp section
        {
            rmx = glm::rotate(glm::mat4(1.0f), _rot.x, glm::vec3(1,0,0));
        }
        #pragma omp section
        {
            rmy = glm::rotate(glm::mat4(1.0f), _rot.y, glm::vec3(0,1,0));
        }
        #pragma omp section
        {
            rmz = glm::rotate(glm::mat4(1.0f), _rot.z, glm::vec3(0,0,1));
        }
        #pragma omp section
        {
            sm = glm::scale(glm::mat4(1.0f), _scale);
        }
    }
}
    _mat = tm*(rmx*rmy*rmz)*sm;
#else
    _mat = ( glm::translate(glm::mat4(1.0f), _pos) * ( glm::rotate(glm::mat4(1.0f), _rot.x, glm::vec3(1,0,0)) * glm::rotate(glm::mat4(1.0f), _rot.y, glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1.0f), _rot.z, glm::vec3(0,0,1)) ) * glm::scale(glm::mat4(1.0f), _scale) );
#endif // MR_USE_OPENMP
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
#ifdef MR_USE_OPENMP
    float a, b;
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            a = glm::cos( r );
        }
        #pragma omp section
        {
            b = glm::sin( r );
        }
    }
}
    return glm::vec2( a , b );
#else
    return glm::vec2( glm::cos( r ), glm::sin( r ) );
#endif // MR_USE_OPENMP
}

glm::vec3 Transform::NormalizedDirection(float angleX, float angleY, float angleZ) {
    float rx = glm::radians(angleX);
    float ry = glm::radians(angleY);
#ifdef MR_USE_OPENMP
    float a, b, c;
#pragma omp parallel
{
    #pragma omp sections
    {
        #pragma omp section
        {
            a = glm::cos( ry ) * glm::sin( rx );
        }
        #pragma omp section
        {
            b = glm::sin( ry );
        }
        #pragma omp section
        {
            c = glm::cos( ry ) * glm::cos( rx );
        }
    }
}
    return glm::vec3(a, b, c);
#else
    return glm::vec3(
        glm::cos( ry ) * glm::sin( rx ),
        glm::sin( ry ),
        glm::cos( ry ) * glm::cos( rx )
    );
#endif
}

}
