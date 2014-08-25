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

    void* GetMappedMemory() override { return _mapped_mem; }
    void Allocate(const Usage& usage, const size_t& size, bool mapMemory) override;
    bool BufferData(void* data, const size_t& offset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) override;
    bool MapMemory() override;
    void UnMapMemory() override;

    /* GPUObjectHandle */
    //unsigned int GetGPUHandle() override;
    size_t GetGPUMem() override { return _size; }

    /* ObjectHandle */
    void Destroy() override;

    GPUBuffer();
    virtual ~GPUBuffer();
protected:
    void* _mapped_mem;
    BindTargets _bindedTarget;
    size_t _size;
    unsigned int _mappingFlags;
};

IGPUBuffer* GPUBufferGetBinded(const IGPUBuffer::BindTargets& target);
void GPUBufferUnBind(const IGPUBuffer::BindTargets& target); //BindBuffer(0)
void GPUBufferUnBindAt(const IGPUBuffer::BindTargets& target, const unsigned int& index); //BindBufferBase(0)

/** !! DO NOT USE IT ON VirtualGPUBuffers !! **/
void GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size);

}

#endif // _MR_BUFFERS_H_
