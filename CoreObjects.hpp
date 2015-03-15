#pragma once

#include "Types.hpp"

#include "stddef.h"

namespace mr {

class IGPUObjectHandle : public IObjectHandle {
public:
    mu::Event<IGPUObjectHandle*, unsigned int const&> OnGPUHandleChanged;

    virtual unsigned int GetGPUHandle() { return _handle; }
    virtual size_t GetGPUMem() { return 0; }

    virtual ~IGPUObjectHandle();

    /* IObjectHandle */
    //void Destroy();
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
