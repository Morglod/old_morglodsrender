#pragma once

#ifndef _MR_STREAMING_BUFFER_H_
#define _MR_STREAMING_BUFFER_H_

#include "Buffers.hpp"

namespace MR {

class StreamingGPUBuffer : public GPUBuffer {
public:
    /* IGPUBuffer */
    virtual void Allocate(const size_t& size, const bool& mapMemory) { Allocate(IGPUBuffer::Draw, size, mapMemory); }

    StreamingGPUBuffer();
    virtual ~StreamingGPUBuffer();
protected:
    /* IGPUBuffer */
    void Allocate(const Usage& usage, const size_t& size, const bool& mapMemory) override;
};

}

#endif // _MR_STREAMING_BUFFER_H_
