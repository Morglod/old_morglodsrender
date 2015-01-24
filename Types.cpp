#include "Types.hpp"
#include <iostream>

namespace mr {

IObjectHandle::~IObjectHandle() {
    ((IObjectHandle*)this)->OnDestroy(((IObjectHandle*)this));
    ((IObjectHandle*)this)->Destroy();
}

}
