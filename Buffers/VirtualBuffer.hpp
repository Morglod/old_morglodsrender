#pragma once

#ifndef _MR_VIRTUAL_BUFFER_H_
#define _MR_VIRTUAL_BUFFER_H_

#include <Containers.hpp>
#include "BuffersInterfaces.hpp"

namespace mr {

class VirtualGPUBuffer_DestroyEvent;

/**
    It uses part of GPUBuffer as unique buffer
**/
class VirtualGPUBuffer : public IGPUBuffer {
    friend class VirtualGPUBuffer_DestroyEvent;
public:
    /* IGPUBuffer */
    inline void Bind(const BindTarget& target) override { _realBuffer->Bind(target); }
    inline void BindAt(const BindTarget& target, const unsigned int& index) override { _realBuffer->BindAt(target, index); }
    inline IGPUBuffer* ReBind(const BindTarget& target) override { return _realBuffer->ReBind(target); }
    inline BindTarget GetBindTarget() override { return _realBuffer->GetBindTarget(); }
    inline bool Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) override { return _realBuffer->Write(srcData, srcOffset, dstOffset+GetRealOffset(), size, out_realOffset, out_info); }
    inline bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override { return _realBuffer->Read(dstData, dstOffset, srcOffset+GetRealOffset(), size); }
    inline Usage GetUsage() override { return _realBuffer->GetUsage(); }

    inline IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) override { return _realBuffer->Map(offset + _realBuffer_offset, length, flags); }
    inline IMappedRangeWeakPtr GetMapped() { return _realBuffer->GetMapped(); }
    inline bool IsMapped() { return _realBuffer->IsMapped(); }

    /* GPUObjectHandle */
    inline unsigned int GetGPUHandle() override { return (_realBuffer) ? _realBuffer->GetGPUHandle() : 0; }
    inline size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    inline virtual IGPUBuffer* GetRealBuffer() { return _realBuffer; }
    inline virtual size_t GetRealOffset() { return _realBuffer_offset; }

    VirtualGPUBuffer(IGPUBuffer* realBuffer, size_t const& offset, size_t const& size);
    virtual ~VirtualGPUBuffer();
protected:
    /* IGPUBuffer */
    void Allocate(const Usage& usage, const size_t& size) override {}

    IGPUBuffer* _realBuffer;
    size_t _realBuffer_offset;
    size_t _size;
    size_t _eventHandle;
};

class VirtualGPUBufferManager {
public:
    mu::Event<VirtualGPUBufferManager*> OnDelete; //before deletion

    inline virtual size_t GetFreeMemorySize() { return _realBuffer->GetGPUMem() - _offset; }
    inline virtual size_t GetOffset() { return _offset; }

    virtual VirtualGPUBuffer* Take(const size_t& size);
    virtual VirtualGPUBuffer* TakeAll() { return Take(GetFreeMemorySize()); }
    inline virtual mu::ArrayRef<VirtualGPUBuffer*> GetAllBuffers() { return mu::ArrayRef<VirtualGPUBuffer*>(&_buffers[0], _buffers.size()); }

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
