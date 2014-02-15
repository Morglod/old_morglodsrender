#include "Anim.hpp"

namespace MR{

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
        for(typename _typed_vector::iterator it = _keys.begin(); it != _keys.end(); ++it){ it->time *= MaxTimeMult; }
    } else {
        _anim_time = v;
    }
}

}
