#pragma once

#include "BuffersInterfaces.hpp"
#include <mu/Singleton.hpp>

#include <unordered_set>

namespace mr {

class GPUBufferManager : public mu::StaticSingleton<GPUBufferManager> {
public:
    IGPUBuffer* CreateBuffer(IGPUBuffer::Usage const& usage, size_t const& size);
    inline size_t GetUsedGPUMemory() { return _usedMem; }

    void DestroyAllBuffers();

    GPUBufferManager();
    virtual ~GPUBufferManager();
private:
    void _RegisterBuffer(IGPUBuffer* buf);
    void _UnRegisterBuffer(IGPUBuffer* buf);
    std::unordered_set<IGPUBuffer*> _buffers;
    size_t _usedMem;
};

}
