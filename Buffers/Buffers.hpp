#pragma once

#ifndef _MR_BUFFERS_H_
#define _MR_BUFFERS_H_

#include "BuffersInterfaces.hpp"

namespace MR {

class GPUBuffer : public IGPUBuffer {
public:
    /* IGPUBuffer */
    void Bind(const BindTargets& target) override;
    void BindAt(const BindTargets& target, const unsigned int& index) override;
    IGPUBuffer* ReBind(const BindTargets& target) override;
    BindTargets GetTarget() override;

    void Allocate(const Usage& usage, const size_t& size) override;
    inline Usage GetUsage() override { return _usage; }
    bool Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) override;
    bool Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) override;

    /* GPUObjectHandle */
    //unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    GPUBuffer();
    virtual ~GPUBuffer();
protected:
    BindTargets _bindedTarget;
    size_t _size;
    Usage _usage;
};

IGPUBuffer* GPUBufferGetBinded(const IGPUBuffer::BindTargets& target);
void GPUBufferUnBind(const IGPUBuffer::BindTargets& target); //BindBuffer(0)
void GPUBufferUnBindAt(const IGPUBuffer::BindTargets& target, const unsigned int& index); //BindBufferBase(0)

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
void GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

}

#endif // _MR_BUFFERS_H_
