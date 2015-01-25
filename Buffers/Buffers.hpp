#pragma once

#ifndef _MR_BUFFERS_H_
#define _MR_BUFFERS_H_

#include "BuffersInterfaces.hpp"

namespace mr {

class GPUBuffer : public IGPUBuffer {
public:
    /* IGPUBuffer */
    void Bind(const BindTarget& target) override;
    void BindAt(const BindTarget& target, const unsigned int& index) override;
    IGPUBuffer* ReBind(const BindTarget& target) override;
    BindTarget GetBindTarget() override;

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
    BindTarget _bindedTarget;
    size_t _size;
    Usage _usage;
};

IGPUBuffer* GPUBufferGetBinded(const IGPUBuffer::BindTarget& target);
void GPUBufferUnBind(const IGPUBuffer::BindTarget& target); //BindBuffer(0)
void GPUBufferUnBindAt(const IGPUBuffer::BindTarget& target, const unsigned int& index); //BindBufferBase(0)

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
void GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

void DestroyAllBuffers();

}

#endif // _MR_BUFFERS_H_
