#include "ShaderUBO.hpp"

#include "../Buffers/BuffersInterfaces.hpp"
#include "../Buffers/BufferManager.hpp"

#include "../Utils/Log.hpp"

namespace mr {

size_t ShaderUBO::GetGPUMem() {
    return (_buffer == nullptr) ? 0 : _buffer->GetGPUMem();
}

unsigned int ShaderUBO::GetGPUHandle() {
    return (_buffer == nullptr) ? 0 : _buffer->GetGPUHandle();
}

void ShaderUBO::Destroy() {
    auto& mgr = GPUBufferManager::GetInstance();
    mgr.Delete(_buffer);
}

bool ShaderUBO::SetData(void* data, size_t const& size, bool dynamic) {
    if(_buffer == nullptr) {
        if(!_CreateBuffer(size, dynamic)) return false;
    }
    return _buffer->Write(data, 0, 0, size, nullptr, nullptr);
}

bool ShaderUBO::_CreateBuffer(size_t const& size, bool dynamic) {
    if(_buffer != nullptr && _buffer->GetGPUMem() >= size) return true;

    auto& mgr = GPUBufferManager::GetInstance();

    //Reallocate if not enought memory
    if(_buffer->GetGPUMem() < size) {
        mgr.Delete(_buffer);
    }
    _buffer = mgr.CreateBuffer((dynamic) ? IGPUBuffer::Usage::FastChange : IGPUBuffer::Usage::Static, size);

    //Failed create buffer
    if(_buffer == nullptr) {
        mr::Log::LogString("ShaderUBO::_CreateBuffer failed. Faield create buffer.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    return true;
}

ShaderUBO::~ShaderUBO() {}

}
