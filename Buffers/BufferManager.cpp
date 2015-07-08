#include "BufferManager.hpp"
#include "Buffers.hpp"

namespace mr {

template<>
mr::BufferManager mu::StaticSingleton<mr::BufferManager>::_singleton_instance = mr::BufferManager();

IBuffer* BufferManager::CreateBuffer(size_t const& size, BufferUsage const& usage) {
    IBuffer* buffer = dynamic_cast<IBuffer*>(new Buffer());

    if(!buffer->Allocate(usage, size)) {
        delete buffer;
        return nullptr;
    }

    class COnGPUBufferAllocated : public mu::Event<IBuffer*, size_t const&>::Listener {
    public:
        void Callback(IBuffer*, size_t const& sz) override {
            BufferManager::GetInstance()._usedMem += sz;
        }

        COnGPUBufferAllocated() {}
        virtual ~COnGPUBufferAllocated() { }
    };

    class COnGPUBufferDestroy : public mu::Event<IGPUObjectHandle*>::Listener {
    public:
        IBuffer* currentBuffer = nullptr;

        virtual ~COnGPUBufferDestroy() {
        }

        void Callback(IGPUObjectHandle* obj) override {
            BufferManager::GetInstance()._usedMem -= currentBuffer->GetGPUMem();
        }

        COnGPUBufferDestroy(IBuffer* curBuf) : currentBuffer(curBuf) {}
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

BufferManager::BufferManager() {
}

BufferManager::~BufferManager() {
}

}
