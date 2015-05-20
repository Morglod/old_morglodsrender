#pragma once

#include "BuffersInterfaces.hpp"

namespace mr {

class GPUBufferRangeHandle : public IGPUBufferRangeHandle {
    friend class GPUBuffer;
public:
    inline IGPUBuffer* GetBuffer() override { return _buffer; }
    inline size_t GetOffset() override { return _offset; }
    inline size_t GetSize() override { return _size; }

    virtual ~GPUBufferRangeHandle();
protected:
    GPUBufferRangeHandle(IGPUBuffer* buf, size_t const& off, size_t const& sz);
    IGPUBuffer* _buffer;
    size_t _offset;
    size_t _size;
};

class GPUBuffer : public IGPUBuffer {
    friend class GPUBufferManager;
public:
    inline Usage GetUsage() override { return _usage; }
    bool Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset, BufferedDataInfo* __restrict__ out_info) override;
    bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override;

    IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) override;
    inline IMappedRangeWeakPtr GetMapped() override { return _mapped; }

    inline bool IsMapped() override {
        if(_mapped.expired()) return false;
        auto ptr = _mapped.lock();
        if((ptr == nullptr) || (ptr->IsMapped() == false)) {
            _mapped = IMappedRangeWeakPtr();
            return false;
        }
        return true;
    }

    IGPUBufferRangeHandleWeakPtr UseRange(size_t const& offset, size_t const& size) override;
    mu::ArrayHandle<IGPUBufferRangeHandle*> GetRangeHandles() override;

    //-
    bool MakeResident() override;
    void MakeNonResident() override;
    inline bool GetGPUAddress(uint64_t& out) const override {
        if(_residentPtr == 0) return false;
        out = _residentPtr;
        return true;
    }
    bool IsResident() const override {
        return (_residentPtr != 0);
    }
    //-

    /* GPUObjectHandle */
    //unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

protected:
    virtual ~GPUBuffer();
    GPUBuffer();
    bool Allocate(const Usage& usage, const size_t& size) override;
    void _RangeFree(IGPUBufferRangeHandle* handle) override;

    class MappedRange : public IGPUBuffer::IMappedRange {
    public:
        inline IGPUBuffer* GetBuffer() override { return _buffer; }
        inline char* Get() override { return _mem; }
        inline size_t GetLength() override { return _length; }
        inline size_t GetOffset() override { return _offset; }
        inline unsigned int GetFlags() override { return _flags; }
        void Flush() override;
        void UnMap() override;

        bool IsMapped() override { return (_mem != 0); }

        bool Map(GPUBuffer* buf, size_t const& offset, size_t const& length, unsigned int const& flags);

        MappedRange();
        virtual ~MappedRange();
    private:
        IGPUBuffer* _buffer;
        char* _mem;
        size_t _length;
        size_t _offset;
        unsigned int _flags;
    };

    size_t _size;
    Usage _usage;
    IMappedRangeWeakPtr _mapped;
    uint64_t _residentPtr = 0;

    std::vector<IGPUBufferRangeHandlePtr> _rangeHandles;
};

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
bool GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

}
