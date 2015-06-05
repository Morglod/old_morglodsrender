#pragma once

#ifndef _MR_VIRTUAL_BUFFER_H_
#define _MR_VIRTUAL_BUFFER_H_

#include <mu/Containers.hpp>
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
    inline bool Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset, BufferedDataInfo* __restrict__ out_info) override { return _realBuffer->Write(srcData, srcOffset, dstOffset+GetRealOffset(), size, out_realOffset, out_info); }
    inline bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override { return _realBuffer->Read(dstData, dstOffset, srcOffset+GetRealOffset(), size); }
    inline Usage GetUsage() override { return _realBuffer->GetUsage(); }

    inline IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) override { return _realBuffer->Map(offset + _realBuffer_offset, length, flags); }
    inline IMappedRangeWeakPtr GetMapped() override { return _realBuffer->GetMapped(); }
    inline bool IsMapped() override { return _realBuffer->IsMapped(); }

    inline IGPUBufferRangeHandleWeakPtr UseRange(size_t const& offset, size_t const& size) override { return _realBuffer->UseRange(GetRealOffset()+offset, size); }
    inline mu::ArrayHandle<IGPUBufferRangeHandle*> GetRangeHandles() override { return _realBuffer->GetRangeHandles(); }

    inline bool MakeResident() override { return _realBuffer->MakeResident(); }
    inline void MakeNonResident() override { _realBuffer->MakeNonResident(); }
    inline bool GetGPUAddress(uint64_t& out) const override {
        if(!_realBuffer->GetGPUAddress(out)) return false;
        out += GetRealOffset();
        return true;
    }
    bool IsResident() const override { return _realBuffer->IsResident(); }

    /* GPUObjectHandle */
    inline unsigned int GetGPUHandle() override { return (_realBuffer) ? _realBuffer->GetGPUHandle() : 0; }
    inline size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    inline virtual IGPUBuffer* GetRealBuffer() { return _realBuffer; }
    inline virtual size_t GetRealOffset() const { return _realBuffer_offset; }

    VirtualGPUBuffer(IGPUBuffer* realBuffer, size_t const& offset, size_t const& size);
    virtual ~VirtualGPUBuffer();
protected:
    /* IGPUBuffer */
    bool Allocate(const Usage& usage, const size_t& size) override;
    void _RangeFree(IGPUBufferRangeHandle* handle) override { return _realBuffer->_RangeFree(handle); }

    IGPUBuffer* _realBuffer = nullptr;
    size_t _realBuffer_offset = 0;
    size_t _size = 0;
    size_t _eventHandle = 0;
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
