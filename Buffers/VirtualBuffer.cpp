#include "VirtualBuffer.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace MR {
class VirtualGPUBuffer_MemoryMappedEvent : public MR::EventHandle<IGPUBuffer*, void*> {
public:
    void Invoke(EventListener<IGPUBuffer*, void*>* event, IGPUBuffer* buf, void* mem) override {
        if(mem == 0) _vgb->_mapped_mem = 0;
        else _vgb->_mapped_mem = (void*)((size_t)mem + _vgb->_realBuffer_offset);
        _vgb->OnMemoryMapped(dynamic_cast<MR::IGPUBuffer*>(_vgb), _vgb->_mapped_mem);
    }

    VirtualGPUBuffer_MemoryMappedEvent(MR::VirtualGPUBuffer* vgb) : _vgb(vgb) {}
    virtual ~VirtualGPUBuffer_MemoryMappedEvent() {}
private:
    MR::VirtualGPUBuffer* _vgb;
};

class VirtualGPUBuffer_DestroyEvent : public MR::EventHandle<ObjectHandle*> {
public:
    void Invoke(EventListener<ObjectHandle*>* event, ObjectHandle* o) override {
        _vgb->Destroy();
    }

    VirtualGPUBuffer_DestroyEvent(MR::VirtualGPUBuffer* vgb) : _vgb(vgb) {}
    virtual ~VirtualGPUBuffer_DestroyEvent() {}
private:
    MR::VirtualGPUBuffer* _vgb;
};

bool VirtualGPUBuffer::BufferData(void* data, const size_t& offset, const size_t& size, size_t* out_realOffset) {
    if(GetMappedMemory() != 0) return false;
    Assert(data == 0)
    Assert(size == 0)
    Assert(size+offset > GetGPUMem())
    Assert(GetGPUHandle() == 0)

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            glNamedBufferSubDataEXT(GetGPUHandle(), GetRealOffset()+offset, size, data);
        } else {
            IGPUBuffer* binded = ReBind(ArrayBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, GetRealOffset()+offset, size, data);
            if(binded) binded->Bind(ArrayBuffer);
        }
    )

    if(out_realOffset) *out_realOffset = GetRealOffset()+offset;
    return true;
}

void VirtualGPUBuffer::Destroy() {
    _mapped_mem = 0;
    _size = 0;
    _realBuffer_offset = 0;
    _realBuffer = nullptr;
    OnDestroy(dynamic_cast<MR::IGPUBuffer*>(this));
}

VirtualGPUBuffer::VirtualGPUBuffer(IGPUBuffer* realBuffer, size_t const& offset, size_t const& size) : _realBuffer(realBuffer), _realBuffer_offset(offset), _mapped_mem(0), _size(size) {
    Assert(realBuffer == 0)
    Assert(size == 0)
    Assert(offset >= realBuffer->GetGPUMem())

    if(realBuffer->GetGPUMem() == 0) {
        _mapped_mem = 0;
        MR::Log::LogString("VirtualGPUBuffer::ctor. RealBuffer not mapped.", MR_LOG_LEVEL_WARNING);
    }
    else _mapped_mem = (void*)(realBuffer->GetGPUMem() + offset);
    realBuffer->OnMemoryMapped.RegisterHandle(new VirtualGPUBuffer_MemoryMappedEvent(this));
    realBuffer->OnDestroy.RegisterHandle(new VirtualGPUBuffer_DestroyEvent(this));
}

VirtualGPUBuffer::~VirtualGPUBuffer() {
}

/** MANAGER **/

VirtualGPUBuffer* VirtualGPUBufferManager::Take(const size_t& size) {
    Assert(size == 0)
    Assert(size+_offset > _realBuffer->GetGPUMem())
    Assert(_realBuffer->GetGPUHandle() == 0)

    VirtualGPUBuffer* buf = new MR::VirtualGPUBuffer(_realBuffer, _offset, size);
    _offset += size;
    _buffers.push_back(buf);
    return buf;
}

VirtualGPUBufferManager::VirtualGPUBufferManager(IGPUBuffer* realBuffer, const size_t& offset) : _realBuffer(realBuffer), _offset(offset) {
}

VirtualGPUBufferManager::~VirtualGPUBufferManager() {
    OnDelete(this);
    while(!_buffers.empty()) {
        _buffers.back()->Destroy();
        delete _buffers.back();
        _buffers.pop_back();
    }
}

}
