#pragma once

#include "BuffersInterfaces.hpp"
#include <mu/Singleton.hpp>

#include "../GPUObjectManager.hpp"

namespace mr {

class GPUBufferManager : public TGPUObjectManager<IGPUBuffer>, public mu::StaticSingleton<GPUBufferManager> {
public:
    IGPUBuffer* CreateBuffer(IGPUBuffer::Usage const& usage, size_t const& size);

    GPUBufferManager();
    virtual ~GPUBufferManager();
};

}
