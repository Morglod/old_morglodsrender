#include "BuffersManager.hpp"
#include "Buffers.hpp"

namespace mr {

template<>
mr::GPUBuffersManager mu::StaticSingleton<mr::GPUBuffersManager>::_singleton_instance = mr::GPUBuffersManager();

IGPUBuffer* GPUBuffersManager::CreateBuffer(IGPUBuffer::Usage const& usage, size_t const& size) {
    IGPUBuffer* buffer = dynamic_cast<IGPUBuffer*>(new GPUBuffer());

    if(!buffer->Allocate(usage, size)) {
        delete buffer;
        return nullptr;
    }

    class COnGPUBufferAllocated : public mu::Event<IGPUBuffer*, size_t const&>::Listener {
    public:
        void Callback(IGPUBuffer*, size_t const& sz) override {
            GPUBuffersManager::GetInstance()._usedMem += sz;
        }

        COnGPUBufferAllocated() {}
        virtual ~COnGPUBufferAllocated() { }
    };

    class COnGPUBufferDestroy : public mu::Event<IGPUObjectHandle*>::Listener {
    public:
        IGPUBuffer* currentBuffer = nullptr;

        virtual ~COnGPUBufferDestroy() {
            GPUBuffersManager::GetInstance()._UnRegisterBuffer(currentBuffer);
        }

        void Callback(IGPUObjectHandle* obj) override {
            GPUBuffersManager::GetInstance()._usedMem -= currentBuffer->GetGPUMem();
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
    _RegisterBuffer(buffer);

    ///GPUBuffer is already allocated, so OnGPUBufferAllocated event isn't Invoked.
    buffer->OnGPUBufferAllocated.Invoke(buffer, size);

    return buffer;
}

void GPUBuffersManager::_RegisterBuffer(IGPUBuffer* buf) {
    _buffers.insert(buf);
}

void GPUBuffersManager::_UnRegisterBuffer(IGPUBuffer* buf) {
    _buffers.erase(buf);
}

void GPUBuffersManager::DestroyAllBuffers() {
    for(IGPUBuffer* buf : _buffers) buf->Destroy();
    _buffers.clear();
}

GPUBuffersManager::GPUBuffersManager() : _usedMem(0) {
}

GPUBuffersManager::~GPUBuffersManager() {
    DestroyAllBuffers();
}

}
