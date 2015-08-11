#pragma once

#include <mu/Events.hpp>
#include "stddef.h"

namespace mr {

class IGPUObjectHandle {
public:
    mu::Event<IGPUObjectHandle*, unsigned int const&> OnGPUHandleChanged;
    mu::Event<IGPUObjectHandle*> OnDestroy;

    virtual unsigned int GetGPUHandle() { return _handle; }
    virtual size_t GetGPUMem() { return 0; }
    virtual void Destroy();

    virtual ~IGPUObjectHandle();
protected:
    virtual void SetGPUHandle(unsigned int const& newHandle) {
        _handle = newHandle;
        OnGPUHandleChanged.Invoke(this, newHandle);
    }
private:
    unsigned int _handle = 0;
};

typedef std::shared_ptr<IGPUObjectHandle> GPUObjectHandlePtr;

}
