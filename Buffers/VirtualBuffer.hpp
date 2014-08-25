#pragma once

#ifndef _MR_VIRTUAL_BUFFER_H_
#define _MR_VIRTUAL_BUFFER_H_

#include "BuffersInterfaces.hpp"

namespace MR {

class VirtualGPUBuffer_MemoryMappedEvent;
class VirtualGPUBuffer_DestroyEvent;

/**
    It uses part of GPUBuffer as unique buffer
**/
class VirtualGPUBuffer : public IGPUBuffer {
    friend class VirtualGPUBuffer_MemoryMappedEvent;
    friend class VirtualGPUBuffer_DestroyEvent;
public:
    /* IGPUBuffer */
    void Bind(const BindTargets& target) override { _realBuffer->Bind(target); }
    void BindAt(const BindTargets& target, const unsigned int& index) override { _realBuffer->BindAt(target, index); }
    IGPUBuffer* ReBind(const BindTargets& target) override { return _realBuffer->ReBind(target); }
    BindTargets GetTarget() override { return _realBuffer->GetTarget(); }
    void* GetMappedMemory() override { return _mapped_mem; }
    bool BufferData(void* data, const size_t& offset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) override;
    inline bool MapMemory() override { return _realBuffer->MapMemory(); }
    inline void UnMapMemory() override { _realBuffer->UnMapMemory(); }

    /* GPUObjectHandle */
    unsigned int GetGPUHandle() override { return (_realBuffer) ? _realBuffer->GetGPUHandle() : 0; }
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    inline virtual IGPUBuffer* GetRealBuffer() { return _realBuffer; }
    inline virtual size_t GetRealOffset() { return _realBuffer_offset; }

    VirtualGPUBuffer(IGPUBuffer* realBuffer, size_t const& offset, size_t const& size);
    virtual ~VirtualGPUBuffer();
protected:
    /* IGPUBuffer */
    void Allocate(const Usage& usage, const size_t& size, bool mapMemory) override {}

    IGPUBuffer* _realBuffer;
    size_t _realBuffer_offset;

    void* _mapped_mem;
    size_t _size;

    void* _events[2];
};

class VirtualGPUBufferManager {
public:
    MR::EventListener<VirtualGPUBufferManager*> OnDelete; //before deletion

    inline virtual size_t GetFreeMemorySize() { return _realBuffer->GetGPUMem() - _offset; }
    inline virtual size_t GetOffset() { return _offset; }

    virtual VirtualGPUBuffer* Take(const size_t& size);
    virtual VirtualGPUBuffer* TakeAll() { return Take(GetFreeMemorySize()); }
    inline virtual MR::StaticArray<VirtualGPUBuffer*> GetAllBuffers() { return MR::StaticArray<VirtualGPUBuffer*>(&_buffers[0], _buffers.size(), false); }

    inline virtual IGPUBuffer* GetRealBuffer() { return _realBuffer; }

    VirtualGPUBufferManager(IGPUBuffer* realBuffer, const size_t& offset);
    virtual ~VirtualGPUBufferManager();
protected:
    IGPUBuffer* _realBuffer;
    size_t _offset;
    std::vector<VirtualGPUBuffer*> _buffers;
};

}

#endif // _MR_STREAMING_BUFFER_H_
