#pragma once

#include "BuffersInterfaces.hpp"
#include <Singleton.hpp>

#include <unordered_set>

namespace mr {

class GPUBuffersManager : public mu::StaticSingleton<GPUBuffersManager> {
public:
    IGPUBuffer* CreateBuffer();
    void RegisterBuffer(IGPUBuffer* buf);
    void UnRegisterBuffer(IGPUBuffer* buf);

    void DestroyAllBuffers();

    GPUBuffersManager();
    virtual ~GPUBuffersManager();
private:
    std::unordered_set<IGPUBuffer*> _buffers;
};

}
