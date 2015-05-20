#pragma once

#include "BuffersConfig.hpp"
#include "../CoreObjects.hpp"
#include <mu/Containers.hpp>

#include <memory>

namespace mr {

class IGPUBuffer;

//Memory is used, so handle it.
class IGPUBufferRangeHandle {
public:
    virtual IGPUBuffer* GetBuffer() = 0;
    virtual size_t GetOffset() = 0;
    virtual size_t GetSize() = 0;
    virtual bool IsInRange(size_t const& x);
    virtual ~IGPUBufferRangeHandle() {}
protected:
    //Call it in class, derived from IGPUBufferRangeHandle, when yu free this memory range.
    virtual void _FreeRange();
};

typedef std::shared_ptr<IGPUBufferRangeHandle> IGPUBufferRangeHandlePtr;
typedef std::weak_ptr<IGPUBufferRangeHandle> IGPUBufferRangeHandleWeakPtr;

class IGPUBuffer : public IGPUObjectHandle {
    friend class GPUBufferManager;
    friend class IGPUBufferRangeHandle;
    friend class VirtualGPUBuffer;
public:

    mu::Event<IGPUBuffer*, size_t const&> OnGPUBufferAllocated;

    enum Usage {
        Static = 0,
        FastChange = 1,
        FastReadWrite = 2
    };

    struct BufferedDataInfo {
    public:
        IGPUBuffer* buffer;
        size_t offset, size;
        BufferedDataInfo() : buffer(nullptr), offset(0), size(0) {}
        BufferedDataInfo(IGPUBuffer* b, size_t const& o, size_t const& s) : buffer(b), offset(o), size(s) {}
    };

    class IMappedRange {
    public:
        //Example: (Write | FlushExplicit)
        enum Flags {
            Read = 0x0001,
            Write = 0x0002,
            Invalidate = 0x0004, //Only with Write
            FlushExplicit = 0x0010, //Only with Write
            Unsynchronized = 0x0020,

            //OpenGL 4.4
            Persistent = 0x00000040,
            Coherent = 0x00000080
        };

        virtual IGPUBuffer* GetBuffer() = 0;
        virtual char* Get() = 0;
        virtual size_t GetLength() = 0;
        virtual size_t GetOffset() = 0;
        virtual unsigned int GetFlags() = 0;
        virtual void Flush() = 0;
        virtual void UnMap() = 0;
        virtual bool IsMapped() = 0;
        virtual ~IMappedRange() {}
    };

    typedef std::shared_ptr<IMappedRange> IMappedRangePtr;
    typedef std::weak_ptr<IMappedRange> IMappedRangeWeakPtr;

    virtual Usage GetUsage() = 0;

    /*  out_realOffset is offset of buffered data. out_realOffset pointer and out_info may be nullptr
        false will be returned if memory is mapped */
    virtual bool Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset, BufferedDataInfo* __restrict__ out_info) = 0;
    virtual bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) = 0;

    virtual IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) = 0;
    virtual IMappedRangeWeakPtr GetMapped() = 0;
    virtual bool IsMapped() = 0;

    virtual IGPUBufferRangeHandleWeakPtr UseRange(size_t const& offset, size_t const& size) = 0;
    virtual mu::ArrayHandle<IGPUBufferRangeHandle*> GetRangeHandles() = 0;

    //NV_shader_buffer_load should be supported
    ///Unlock buffer, before massive changes (e.g. SetData)
    virtual bool MakeResident() = 0;
    virtual void MakeNonResident() = 0;
    virtual bool GetGPUAddress(uint64_t& out) const = 0;
    virtual bool IsResident() const = 0;
    //-

protected:
    virtual bool Allocate(const IGPUBuffer::Usage& usage, const size_t& size) = 0;
    virtual void _RangeFree(IGPUBufferRangeHandle* handle) = 0;
};

}

inline void mr::IGPUBufferRangeHandle::_FreeRange() {
    GetBuffer()->_RangeFree(this);
}

inline bool mr::IGPUBufferRangeHandle::IsInRange(size_t const& x) {
    if(x < GetOffset()) return false;
    if(x >= GetOffset()+GetSize()) return false;
    return true;
}
