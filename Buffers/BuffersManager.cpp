#include "BuffersManager.hpp"
#include "Buffers.hpp"

namespace mr {

template<>
mr::GPUBuffersManager mu::StaticSingleton<mr::GPUBuffersManager>::_singleton_instance = mr::GPUBuffersManager();

IGPUBuffer* GPUBuffersManager::CreateBuffer() {
    IGPUBuffer* buffer = dynamic_cast<IGPUBuffer*>(new GPUBuffer());

    class COnGPUBufferDestroy : public mu::Event<IObjectHandle*>::Listener {
    public:
        IGPUBuffer* currentBuffer = nullptr;

        virtual ~COnGPUBufferDestroy() {
            GPUBuffersManager::GetInstance().UnRegisterBuffer(currentBuffer);
        }

        void Callback(IObjectHandle*) override {}
        COnGPUBufferDestroy(IGPUBuffer* curBuf) : currentBuffer(curBuf) {}
    };

    /**
    Event<>::RegisterListener register listener class as shared_ptr,
    so when buffer instance is deleted, listener class is deleted
    then COnGPUBufferDestroy's destructor is called and buffer instance is unregistered.
    **/

    buffer->OnDestroy.RegisterListener(new COnGPUBufferDestroy(buffer));
    RegisterBuffer(buffer);

    return buffer;
}

void GPUBuffersManager::RegisterBuffer(IGPUBuffer* buf) {
    _buffers.insert(buf);
}

void GPUBuffersManager::UnRegisterBuffer(IGPUBuffer* buf) {
    _buffers.erase(buf);
}

void GPUBuffersManager::DestroyAllBuffers() {
    for(IGPUBuffer* buf : _buffers) buf->Destroy();
    _buffers.clear();
}

GPUBuffersManager::GPUBuffersManager() {
}

GPUBuffersManager::~GPUBuffersManager() {
}

}
