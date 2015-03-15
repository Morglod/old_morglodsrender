#include "Types.hpp"
#include <iostream>

namespace mr {

void IObjectHandle::Destroy() {
}

IObjectHandle::~IObjectHandle() {
    ((IObjectHandle*)this)->OnDestroy(((IObjectHandle*)this));
    ((IObjectHandle*)this)->Destroy();
}

}
