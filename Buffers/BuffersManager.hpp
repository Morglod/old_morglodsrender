#pragma once

#include "BuffersInterfaces.hpp"
#include <Singleton.hpp>

#include <unordered_set>

namespace mr {

class GPUBuffersManager : public mu::StaticSingleton<GPUBuffersManager> {
public:
    IGPUBuffer* CreateBuffer(IGPUBuffer::Usage const& usage, size_t const& size);
    inline size_t GetUsedGPUMemory() { return _usedMem; }

    void DestroyAllBuffers();

    GPUBuffersManager();
    virtual ~GPUBuffersManager();
private:
    void _RegisterBuffer(IGPUBuffer* buf);
    void _UnRegisterBuffer(IGPUBuffer* buf);
    std::unordered_set<IGPUBuffer*> _buffers;
    size_t _usedMem;
};

}
