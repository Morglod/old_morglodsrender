#include "Exception.hpp"

bool throwExceptions = false;
bool throwCritExceptions = false;

namespace MR {
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
