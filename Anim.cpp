#include "Anim.hpp"

namespace mr{

template<typename T>
void AnimDesc<T>::SetAnimTime(const MR_ANIM_TIME_TYPE& v){
    SetAnimTime(v, false);
}

template<typename T>
void AnimDesc<T>::SetActivationTime(const MR_ANIM_TIME_TYPE& v){
    _act_time = v;
}

template<typename T>
void AnimDesc<T>::SetDeltaMult(const MR_ANIM_TIME_TYPE& v){
    _delta_mult = v;
}

template<typename T>
void AnimDesc<T>::SetAnimTime(const MR_ANIM_TIME_TYPE& v, const bool& scaleKeys){
    if(scaleKeys) {
        MR_ANIM_TIME_TYPE MaxTimeMult = v / _anim_time;
        _anim_time = v;
        for(size_t i = 0; i < _keys.size(); ++i){ _keys[i].time *= MaxTimeMult; }
    } else {
        _anim_time = v;
    }
}

}
