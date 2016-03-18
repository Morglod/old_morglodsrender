#pragma once

#include "build.hpp"
#include <inttypes.h>

#define MR_NEW(Type, Args...) mr::_Alloc::New<Type>(Args)
#define MR_NEW_ARRAY(Type, Num) mr::_Alloc::NewArray<Type>(Num)
#define MR_NEW_SHARED(Type, Args...) std::shared_ptr<Type>(MR_NEW(Type, Args), mr::_Deleter<Type>)

namespace mr {

template<class T>
inline void _Deleter(T* what) {
    delete what;
}

class MR_API _Alloc {
public:
    template<typename T, typename... Args>
    inline static T* New(Args... args) {
        //void* m = Alloc(sizeof(T));
        //return new (m) T(args...);
        return new T(args...);
    }

    template<typename T>
    inline static T* NewArray(const size_t num) {
        //void* m = Alloc(sizeof(T) * num);
        //return new (m) T[num];
        return new T[num];
    }

    //static void* Alloc(const size_t sz);
};

}
