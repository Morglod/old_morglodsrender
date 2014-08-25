#pragma once

#ifndef _MR_BUFFERS_INTERFACES_H_
#define _MR_BUFFERS_INTERFACES_H_

#include "BuffersConfig.hpp"

#include "../Utils/Memory.hpp"
#include "../CoreObjects.hpp"
#include "../Utils/Events.hpp"

#include <memory>

namespace MR {

class IGPUBuffer : public GPUObjectHandle {
public:
    MR::EventListener<IGPUBuffer*, void*> OnMemoryMapped; //mappedMemory
    //MR::EventListener<IGPUBuffer*, const IGPUBuffer::Usage&, const size_t&, const bool&> OnAllocated; //usage, size, memoryMapped

    enum BindTargets {
        ArrayBuffer = 0,
        AtomicCounterBuffer = 1,
        CopyReadBuffer,
        CopyWriteBuffer,
        DrawIndirectBuffer,
        DispatchIndirectBuffer,
        ElementArrayBuffer,
        PixelPackBuffer,
        PixelUnpackBuffer,
        QueryBuffer,
        ShaderStorageBuffer,
        TextureBuffer,
        TransformFeedbackBuffer,
        UniformBuffer,

        NotBinded = 255
    };

    enum Usage {
        Static,
        FastWrite,
        FastReadWrite
    };

    MR::EventListener<IGPUBuffer*, const IGPUBuffer::Usage&, const size_t&, const bool&> OnAllocated;

    struct BufferedDataInfo {
    public:
        IGPUBuffer* buffer;
        size_t offset, size;
        BufferedDataInfo() : buffer(nullptr), offset(0), size(0) {}
        BufferedDataInfo(IGPUBuffer* b, size_t const& o, size_t const& s) : buffer(b), offset(o), size(s) {}
    };

	/**
		To unbind buffer, use global MR::GPUBufferUnBind method, defined in 'Buffers/Buffers.hpp'.
	**/

    virtual void Bind(const BindTargets& target) = 0; //BindBuffer
    virtual void BindAt(const BindTargets& target, const unsigned int& index) = 0; //BindBufferBase
    virtual IGPUBuffer* ReBind(const BindTargets& target) = 0; //BindBuffer and return what was binded (may be nullptr)
    virtual BindTargets GetTarget() = 0; //returns last binded target, or NotBinded

    virtual void Allocate(const Usage& usage, const size_t& size, bool mapMemory) = 0;

    /*  out_realOffset is offset of buffered data. out_realOffset pointer and out_info may be nullptr
        false will be returned if memory is mapped */
    virtual bool Write(void* data, const size_t& offset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) = 0;
    virtual bool Read(void* dstData, const size_t& offset, const size_t& size) = 0;

    /*
    DAT STUPID DRIVER DEVELOPERS, nothing works...
    virtual void* GetMappedMemory() = 0; //returns zero, if not mapped
    virtual bool MapMemory() = 0;
    virtual void UnMapMemory() = 0;*/

    virtual ~IGPUBuffer() {}
};

}

#endif // _MR_BUFFERS_INTERFACES_H_
