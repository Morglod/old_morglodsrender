#pragma once

#include "BuffersInterfaces.hpp"
#include <mu/Singleton.hpp>

#include "../TGPUObjectManager.hpp"

namespace mr {

class BufferManager : public TGPUObjectManager<IBuffer>, public mu::StaticSingleton<BufferManager> {
public:
    IBuffer* CreateBuffer(size_t const& size, BufferUsage const& usage);

    BufferManager();
    virtual ~BufferManager();
};

}
