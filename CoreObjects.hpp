#pragma once

#ifndef _MR_CORE_OBJECTS_H_
#define _MR_CORE_OBJECTS_H_

#include "Types.hpp"
#include "stddef.h"

namespace mr {

class GPUObjectHandle : public ObjectHandle {
public:
    mr::EventListener<GPUObjectHandle*, unsigned int const&> OnGPUHandleChanged;

    virtual bool Good() { return (_handle != 0); }
    virtual unsigned int GetGPUHandle() { return _handle; }
    virtual size_t GetGPUMem();

    /* ObjectHandle */
    //void Destroy();
protected:
    unsigned int _handle = 0;
};

}

#endif // _MR_CORE_OBJECTS_H_
