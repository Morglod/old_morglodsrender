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
class VirtualBuffer : public IBuffer {
    friend class VirtualGPUBuffer_DestroyEvent;
public:
    /* IGPUBuffer */
    inline bool Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset, BufferedDataInfo* __restrict__ out_info) override { return _realBuffer->Write(srcData, srcOffset, dstOffset+GetRealOffset(), size, out_realOffset, out_info); }
    inline bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override { return _realBuffer->Read(dstData, dstOffset, srcOffset+GetRealOffset(), size); }
    inline BufferUsage GetUsage() override { return _realBuffer->GetUsage(); }

    inline IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) override { return _realBuffer->Map(offset + _realBuffer_offset, length, flags); }
    inline IMappedRangeWPtr GetMapped() override { return _realBuffer->GetMapped(); }
    inline bool IsMapped() override { return _realBuffer->IsMapped(); }

    inline IBufferRangeHdlWPtr UseRange(size_t const& size, size_t const& offset) override { return _realBuffer->UseRange(size, GetRealOffset()+offset); }
    inline mu::ArrayHandle<IBufferRangeHdl*> GetRangeHandles() override { return _realBuffer->GetRangeHandles(); }

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

    inline virtual IBuffer* GetRealBuffer() { return _realBuffer; }
    inline virtual size_t GetRealOffset() const { return _realBuffer_offset; }

    VirtualBuffer(IBuffer* realBuffer, size_t const& offset, size_t const& size);
    virtual ~VirtualBuffer();
protected:
    /* IGPUBuffer */
    bool Allocate(BufferUsage const& usage, const size_t& size) override;
    void _RangeFree(IBufferRangeHdl* handle) override { return _realBuffer->_RangeFree(handle); }

    IBuffer* _realBuffer = nullptr;
    size_t _realBuffer_offset = 0;
    size_t _size = 0;
    size_t _eventHandle = 0;
};

class VirtualBufferManager {
public:
    mu::Event<VirtualBufferManager*> OnDelete; //before deletion

    inline virtual size_t GetFreeMemorySize() { return _realBuffer->GetGPUMem() - _offset; }
    inline virtual size_t GetOffset() { return _offset; }

    virtual VirtualBuffer* Take(const size_t& size);
    virtual VirtualBuffer* TakeAll() { return Take(GetFreeMemorySize()); }
    inline virtual mu::ArrayRef<VirtualBuffer*> GetAllBuffers() { return mu::ArrayRef<VirtualBuffer*>(&_buffers[0], _buffers.size()); }

    inline virtual IBuffer* GetRealBuffer() { return _realBuffer; }

    VirtualBufferManager(IBuffer* realBuffer, const size_t& offset);
    virtual ~VirtualBufferManager();
protected:
    IBuffer* _realBuffer;
    size_t _offset;
    std::vector<VirtualBuffer*> _buffers;
};

}

#endif // _MR_STREAMING_BUFFER_H_
