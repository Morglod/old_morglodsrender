#include "VirtualBuffer.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

void VirtualBuffer::Destroy() {
    if(_realBuffer) {
        if(_eventHandle) {
            _realBuffer->OnDestroy -= _eventHandle;
        }
    }

    _size = 0;
    _realBuffer_offset = 0;
    _realBuffer = nullptr;
}

VirtualBuffer::VirtualBuffer(IBuffer* realBuffer, size_t const& offset, size_t const& size) : _realBuffer(realBuffer), _realBuffer_offset(offset), _size(size), _eventHandle(0) {
    Assert(_realBuffer != nullptr);
    Assert(size != 0);
    Assert(offset < _realBuffer->GetGPUMem());

    if(_realBuffer->GetGPUMem() == 0) {
        mr::Log::LogString("VirtualGPUBuffer::ctor. RealBuffer not allocated or allocated with some errors.", MR_LOG_LEVEL_WARNING);
    }

    _eventHandle = _realBuffer->OnDestroy +=
    [this](IGPUObjectHandle* oh) {
        this->Destroy();
    };
}

bool VirtualBuffer::Allocate(BufferUsage const& usage, const size_t& size) {
    mr::Log::LogString("Cannot Allocate gpu mem in virtual buffer. VirtualGPUBuffer::Allocate failed.", MR_LOG_LEVEL_ERROR);
    return false;
}

VirtualBuffer::~VirtualBuffer() {
}

/** MANAGER **/

VirtualBuffer* VirtualBufferManager::Take(const size_t& size) {
    AssertAndExec(size != 0, return nullptr);
    AssertAndExec(size+_offset <= _realBuffer->GetGPUMem(), return nullptr);
    AssertAndExec(_realBuffer->GetGPUHandle() != 0, return nullptr);

    VirtualBuffer* buf = new mr::VirtualBuffer(_realBuffer, _offset, size);
    _offset += size;
    _buffers.push_back(buf);
    return buf;
}

VirtualBufferManager::VirtualBufferManager(IBuffer* realBuffer, const size_t& offset) : _realBuffer(realBuffer), _offset(offset) {
}

VirtualBufferManager::~VirtualBufferManager() {
    OnDelete(this);
    while(!_buffers.empty()) {
        _buffers.back()->Destroy();
        delete _buffers.back();
        _buffers.pop_back();
    }
}

}
