#pragma once

#ifndef _MR_ANIM_H_
#define _MR_ANIM_H_

#define MR_ANIM_TIME_TYPE float

#include "pre.hpp"

namespace MR{

template<typename T>
struct AnimValue {
public:
    T v;
    MR_ANIM_TIME_TYPE time;
};

typedef AnimValue<float> AnimValueF;
typedef AnimValue<int> AnimValueI;
typedef AnimValue<glm::vec2> AnimValueV2;
typedef AnimValue<glm::vec3> AnimValueV3;
typedef AnimValue<glm::vec4> AnimValueV4;

template<typename T>
class AnimDesc {
public:
    inline MR_ANIM_TIME_TYPE& GetAnimTime(){return _anim_time;}
    inline MR_ANIM_TIME_TYPE& GetActivationTime(){return _act_time;}
    inline MR_ANIM_TIME_TYPE& GetDeltaMult(){return _delta_mult;}

    void SetAnimTime(const MR_ANIM_TIME_TYPE& v);
    void SetAnimTime(const MR_ANIM_TIME_TYPE& v, const bool& scaleKeys);
    void SetActivationTime(const MR_ANIM_TIME_TYPE& v);
    void SetDeltaMult(const MR_ANIM_TIME_TYPE& v);
protected:
    typedef std::vector<AnimValue<T>> _typed_vector;
    _typed_vector _keys;
    MR_ANIM_TIME_TYPE _anim_time; //total time of anim
    MR_ANIM_TIME_TYPE _act_time; //"current time" value when animation plays first time
    MR_ANIM_TIME_TYPE _delta_mult; //currentTime += delta*mult
};

template<typename T>
class Anim {
    AnimDesc<T>* desc;
};

}

#endif // _MR_ANIM_H_
