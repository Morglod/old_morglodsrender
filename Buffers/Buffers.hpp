#pragma once

#ifndef _MR_BUFFERS_H_
#define _MR_BUFFERS_H_

#include "BuffersInterfaces.hpp"

namespace mr {

class GPUBuffer : public IGPUBuffer {
public:
    bool Allocate(const Usage& usage, const size_t& size) override;
    inline Usage GetUsage() override { return _usage; }
    bool Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) override;
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

    /* GPUObjectHandle */
    //unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    GPUBuffer();
    virtual ~GPUBuffer();
protected:
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
};

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
bool GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

void DestroyAllBuffers();

}

#endif // _MR_BUFFERS_H_
