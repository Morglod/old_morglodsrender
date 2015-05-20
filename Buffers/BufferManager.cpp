#include "BufferManager.hpp"
#include "Buffers.hpp"

namespace mr {

template<>
mr::GPUBufferManager mu::StaticSingleton<mr::GPUBufferManager>::_singleton_instance = mr::GPUBufferManager();

IGPUBuffer* GPUBufferManager::CreateBuffer(IGPUBuffer::Usage const& usage, size_t const& size) {
    IGPUBuffer* buffer = dynamic_cast<IGPUBuffer*>(new GPUBuffer());

    if(!buffer->Allocate(usage, size)) {
        delete buffer;
        return nullptr;
    }

    class COnGPUBufferAllocated : public mu::Event<IGPUBuffer*, size_t const&>::Listener {
    public:
        void Callback(IGPUBuffer*, size_t const& sz) override {
            GPUBufferManager::GetInstance()._usedMem += sz;
        }

        COnGPUBufferAllocated() {}
        virtual ~COnGPUBufferAllocated() { }
    };

    class COnGPUBufferDestroy : public mu::Event<IGPUObjectHandle*>::Listener {
    public:
        IGPUBuffer* currentBuffer = nullptr;

        virtual ~COnGPUBufferDestroy() {
        }

        void Callback(IGPUObjectHandle* obj) override {
            GPUBufferManager::GetInstance()._usedMem -= currentBuffer->GetGPUMem();
        }

        COnGPUBufferDestroy(IGPUBuffer* curBuf) : currentBuffer(curBuf) {}
    };

    /**
    Event<>::RegisterListener register listener class as shared_ptr,
    so when buffer instance is deleted, listener class is deleted
    then COnGPUBufferDestroy's destructor is called and buffer instance is unregistered.
    **/

    buffer->OnGPUBufferAllocated.RegisterListener(new COnGPUBufferAllocated());
    buffer->OnDestroy.RegisterListener(new COnGPUBufferDestroy(buffer));
    _Register(buffer);

    ///GPUBuffer is already allocated, so OnGPUBufferAllocated event isn't Invoked.
    buffer->OnGPUBufferAllocated.Invoke(buffer, size);

    return buffer;
}

GPUBufferManager::GPUBufferManager() {
}

GPUBufferManager::~GPUBufferManager() {
}

}
