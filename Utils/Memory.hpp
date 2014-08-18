#pragma once

#ifndef _MR_MEMORY_H_
#define _MR_MEMORY_H_

#include <stddef.h>

namespace MR {

class IMemoryInfo {
public:
    virtual size_t AvailableSize() = 0;
};

class IMemoryReadable {
public:
    virtual bool Read(void* dst, const size_t& offset, const size_t& size) = 0;
};

class IMemoryWritable {
public:
    virtual bool Write(void* src, const size_t& dstOffset, const size_t& srcSize) = 0;
    virtual bool WriteRange(void* src, const size_t& dstOffset, const size_t& srcSize, const size_t& srcOffset) = 0;
};

}

#endif // _MR_MEMORY_H_
