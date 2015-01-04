#include "VirtualBuffer.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

void VirtualGPUBuffer::Destroy() {
    if(_realBuffer) {
        if(_eventHandle) {
            _realBuffer->OnDestroy -= _eventHandle;
        }
    }

    _size = 0;
    _realBuffer_offset = 0;
    _realBuffer = nullptr;
    OnDestroy(dynamic_cast<mr::IGPUBuffer*>(this));
}

VirtualGPUBuffer::VirtualGPUBuffer(IGPUBuffer* realBuffer, size_t const& offset, size_t const& size) : _realBuffer(realBuffer), _realBuffer_offset(offset), _size(size), _eventHandle(0) {
    Assert(_realBuffer == 0)
    Assert(size == 0)
    Assert(offset >= _realBuffer->GetGPUMem())

    if(_realBuffer->GetGPUMem() == 0) {
        mr::Log::LogString("VirtualGPUBuffer::ctor. RealBuffer not allocated or allocated with some errors.", MR_LOG_LEVEL_WARNING);
    }

    _eventHandle = _realBuffer->OnDestroy +=
    [this](ObjectHandle* oh) {
        this->Destroy();
    };
}

VirtualGPUBuffer::~VirtualGPUBuffer() {
}

/** MANAGER **/

VirtualGPUBuffer* VirtualGPUBufferManager::Take(const size_t& size) {
    Assert(size == 0)
    Assert(size+_offset > _realBuffer->GetGPUMem())
    Assert(_realBuffer->GetGPUHandle() == 0)

    VirtualGPUBuffer* buf = new mr::VirtualGPUBuffer(_realBuffer, _offset, size);
    _offset += size;
    _buffers.push_back(buf);
    return buf;
}

VirtualGPUBufferManager::VirtualGPUBufferManager(IGPUBuffer* realBuffer, const size_t& offset) : _realBuffer(realBuffer), _offset(offset) {
}

VirtualGPUBufferManager::~VirtualGPUBufferManager() {
    OnDelete(this);
    while(!_buffers.empty()) {
        _buffers.back()->Destroy();
        delete _buffers.back();
        _buffers.pop_back();
    }
}

}
