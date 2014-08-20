#pragma once

#ifndef _MR_CORE_OBJECTS_H_
#define _MR_CORE_OBJECTS_H_

#include "Types.hpp"
#include "stddef.h"

namespace MR {

class GPUObjectHandle : public ObjectHandle {
public:
    MR::EventListener<GPUObjectHandle*, unsigned int const&> OnGPUHandleChanged;

    virtual unsigned int GetGPUHandle() { return _handle; }
    virtual size_t GetGPUMem();

    /* ObjectHandle */
    //void Destroy();
protected:
    unsigned int _handle = 0;
};

}

#endif // _MR_CORE_OBJECTS_H_
