#pragma once

#ifndef _MR_ANIM_H_
#define _MR_ANIM_H_

#include "Types.hpp"

#include <vector>

#include "pre_glm.hpp"

namespace mr{

template<typename T>
struct AnimValue {
public:
    T v;
    float time;
};

typedef AnimValue<float> AnimValueF;
typedef AnimValue<int> AnimValueI;
typedef AnimValue<glm::vec2> AnimValueV2;
typedef AnimValue<glm::vec3> AnimValueV3;
typedef AnimValue<glm::vec4> AnimValueV4;

template<typename T>
class AnimDesc {
public:
    inline float GetTotalTime() const { return _anim_time; }
    inline float GetBeginTime() const { return _begin_time; }
    inline float GetSpeed() const { return _speed; }
    inline bool GetKeyAfter(float const& time, AnimValue<T>& out) {
        for(auto& key : _keys) {
            key.time <= time;
            out = key;
            return true;
        }
        return false;
    }
protected:
    std::vector<AnimValue<T>> _keys;
    float _anim_time;
    float _begin_time;
    float _speed; //currentTime += delta*mult
};

template<typename T>
class Anim {
protected:
    typename AnimDesc<T>::Ptr _desc;
};

}

#endif // _MR_ANIM_H_
