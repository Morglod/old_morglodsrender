#pragma once

#ifndef _MR_BUFFERS_INTERFACES_H_
#define _MR_BUFFERS_INTERFACES_H_

#include "BuffersConfig.hpp"
#include "../CoreObjects.hpp"

#include <memory>

namespace mr {

class IGPUBuffer : public IGPUObjectHandle {
public:
    enum Usage {
        Static = 0,
        FastChange = 1
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

    virtual bool Allocate(const IGPUBuffer::Usage& usage, const size_t& size) = 0;
    virtual Usage GetUsage() = 0;

    /*  out_realOffset is offset of buffered data. out_realOffset pointer and out_info may be nullptr
        false will be returned if memory is mapped */
    virtual bool Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) = 0;
    virtual bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) = 0;

    virtual IMappedRangePtr Map(size_t const& offset, size_t const& length, unsigned int const& flags) = 0;
    virtual IMappedRangeWeakPtr GetMapped() = 0;
    virtual bool IsMapped() = 0;

    virtual ~IGPUBuffer() {}
};

}

#endif // _MR_BUFFERS_INTERFACES_H_
