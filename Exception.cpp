#include "Exception.hpp"

bool throwExceptions = false;

namespace MR {
    void ThrowExceptions(const bool& state){
        throwExceptions = state;
    }

    bool ThrowExceptions(){
        return throwExceptions;
    }
}
