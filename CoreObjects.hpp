#pragma once

#include "Types.hpp"
#include "stddef.h"

namespace mr {

class IGPUObjectHandle : public IObjectHandle {
public:
    mu::Event<IGPUObjectHandle*, unsigned int const&> OnGPUHandleChanged;

    virtual bool IsGood() { return (_handle != 0); }
    virtual unsigned int GetGPUHandle() { return _handle; }
    virtual size_t GetGPUMem();

    virtual ~IGPUObjectHandle() {}

    /* ObjectHandle */
    //void Destroy();
protected:
    unsigned int _handle = 0;
};

typedef std::shared_ptr<IGPUObjectHandle> GPUObjectHandlePtr;

}
