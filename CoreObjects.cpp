#include "CoreObjects.hpp"

namespace mr {

void IGPUObjectHandle::Destroy() {
    OnDestroy(this);
}

IGPUObjectHandle::~IGPUObjectHandle() {
    if(GetGPUMem() > 0) Destroy();
}

}
