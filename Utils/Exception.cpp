#include "Exception.hpp"

bool throwExceptions = false;
bool throwCritExceptions = false;

namespace mr {
    void ThrowExceptions(const bool& state){
        throwExceptions = state;
    }

    void ThrowCriticalExceptions(const bool& state){
        throwCritExceptions = state;
    }

    bool ThrowExceptions(){
        return throwExceptions;
    }

    bool ThrowCriticalExceptions(){
        return throwCritExceptions;
    }
}
