#pragma once

#include "BuffersInterfaces.hpp"

namespace mr {

class BufferRangeHdl : public IBufferRangeHdl {
    friend class Buffer;
public:
    inline IBuffer* GetBuffer() const override { return _buffer; }
    inline size_t GetOffset() const override { return _offset; }
    inline size_t GetSize() const override { return _size; }
    inline void _StopHandle() override { _disposed = true; }

    virtual ~BufferRangeHdl();
protected:
    BufferRangeHdl(IBuffer* buf, size_t const& off, size_t const& sz);
    IBuffer* _buffer;
    size_t _offset;
    size_t _size;
    bool _disposed = false;
};

class Buffer : public IBuffer {
    friend class BufferManager;
public:
    inline BufferUsage GetUsage() override { return _usage; }
    bool Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset = nullptr, BufferedDataInfo* __restrict__ out_info = nullptr) override;
    bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override;

    IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) override;
    inline IMappedRangeWPtr GetMapped() override { return _mapped; }

    inline bool IsMapped() override {
        if(_mapped.expired()) return false;
        auto ptr = _mapped.lock();
        if((ptr == nullptr) || (ptr->IsMapped() == false)) {
            _mapped = IMappedRangeWPtr();
            return false;
        }
        return true;
    }

    IBufferRangeHdlWPtr UseRange(size_t const& size, size_t const& offset = 0) override; //todo swap offset and size, offset = 0
    mu::ArrayHandle<IBufferRangeHdl*> GetRangeHandles() override;

    //-
    bool MakeResident() override;
    void MakeNonResident() override;
    inline bool GetGPUAddress(uint64_t& out) const override {
        if(_residentPtr == 0) return false;
        out = _residentPtr;
        return true;
    }
    inline bool IsResident() const override { return (_residentPtr != 0); }
    //-

    /* GPUObjectHandle */
    //unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

protected:
    virtual ~Buffer();
    Buffer();
    bool Allocate(BufferUsage const& usage, const size_t& size) override;
    void _RangeFree(IBufferRangeHdl* handle) override;

    class MappedRange : public IBuffer::IMappedRange {
    public:
        inline IBuffer* GetBuffer() override { return _buffer; }
        inline char* Get() override { return _mem; }
        inline size_t GetLength() override { return _length; }
        inline size_t GetOffset() override { return _offset; }
        inline unsigned int GetFlags() override { return _flags; }
        void Flush() override;
        void UnMap() override;

        bool IsMapped() override { return (_mem != 0); }

        bool Map(Buffer* buf, size_t const& offset, size_t const& length, unsigned int const& flags);

        MappedRange();
        virtual ~MappedRange();
    private:
        IBuffer* _buffer;
        char* _mem;
        size_t _length;
        size_t _offset;
        unsigned int _flags;
    };

    uint64_t _residentPtr = 0;
    size_t _size;
    IMappedRangeWPtr _mapped;
    BufferUsage _usage;

    std::vector<IBufferRangeHdlPtr> _rangeHandles;
};

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
bool GPUBufferCopy(IBuffer* src, IBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

}
