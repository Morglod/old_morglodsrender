#include "Buffers.hpp"
#include "../Utils/Containers.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#define MR_BUFFERS_BIND_TARGETS_NUM 14
MR::StaticArray<MR::IGPUBuffer*> _MR_BUFFERS_BIND_TARGETS_(new MR::IGPUBuffer*[MR_BUFFERS_BIND_TARGETS_NUM], MR_BUFFERS_BIND_TARGETS_NUM, true);

class _MR_BUFFERS_BIND_TARGETS_NULL_ {
public:
    _MR_BUFFERS_BIND_TARGETS_NULL_() {
        for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i){
            _MR_BUFFERS_BIND_TARGETS_.GetRaw()[i] = nullptr;
        }
    }
};
_MR_BUFFERS_BIND_TARGETS_NULL_ _MR_BUFFERS_BIND_TARGETS_NULL_DEF_;

size_t _MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[] {
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_QUERY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER,
    GL_TRANSFORM_FEEDBACK_BUFFER,
    GL_UNIFORM_BUFFER
};

/** GPUBuffer class implementation **/

namespace MR {

void GPUBuffer::Bind(const IGPUBuffer::BindTargets& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)

    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] == dynamic_cast<MR::IGPUBuffer*>(this)) return;
    _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = dynamic_cast<MR::IGPUBuffer*>(this);
    _bindedTarget = target;

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle());
    )
}

void GPUBuffer::BindAt(const BindTargets& target, const unsigned int& index) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)

    _bindedTarget = target;

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle(), index);
    )
}

IGPUBuffer* GPUBuffer::ReBind(const IGPUBuffer::BindTargets& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return 0;
    }

    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)

    IGPUBuffer* binded = _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target];
    Bind(target);
    return binded;
}

IGPUBuffer::BindTargets GPUBuffer::GetTarget() {
    return _bindedTarget;
}

void GPUBuffer::Allocate(const Usage& usage, const size_t& size, bool mapMemory) {
    Assert(size == 0)
    if(_handle == 0) {
        glGenBuffers(1, &_handle);
        OnGPUHandleChanged(dynamic_cast<MR::GPUObjectHandle*>(this), _handle);
    }

    if(_size > size) return; //Current size is enough
    _size = size;

    unsigned int usageFlags = 0;
    _mappingFlags = 0;
    switch(usage) {
    case Static:
        usageFlags = GL_STATIC_DRAW;
        _mappingFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        break;
    case FastWrite:
        _mappingFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        usageFlags = GL_STREAM_DRAW;
        break;
    case FastReadWrite:
        _mappingFlags = GL_MAP_WRITE_BIT | GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        usageFlags = GL_STREAM_DRAW;
        break;
    default:
        MR::Log::LogString("GPUBuffer::Allocate. Unknown usage option ("+std::to_string((int)usage)+"), Static will be used.", MR_LOG_LEVEL_WARNING);
        usageFlags = GL_STATIC_DRAW;
        _mappingFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        break;
    }
    if(usage == FastWrite) {
        _mappingFlags = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT;
        usageFlags = GL_STREAM_DRAW;
    }

    IGPUBuffer* binded = 0;
    if(!MR::MachineInfo::IsDirectStateAccessSupported()) binded = ReBind(ArrayBuffer);

    if(mapMemory && !MR::MachineInfo::IsBufferStorageSupported()) {
        MR::Log::LogString("Can not map memory (BufferStorage ext not supported). Regular BufferData will be used.", MR_LOG_LEVEL_WARNING);
        mapMemory = false;
    }
    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        /*if(mapMemory) {
            if(MR::MachineInfo::IsDirectStateAccessSupported()) {
                glNamedBufferStorageEXT(_handle, _size, 0, _mappingFlags);
            } else {
                binded = ReBind(ArrayBuffer);
                MR::Log::LogString("BUFFERSTORAGE", MR_LOG_LEVEL_ERROR);
                glBufferStorage(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], _size, 0, _mappingFlags);
            }
            if(!MapMemory()) {
                MR::Log::LogString("Failed MapMemory in GPUBuffer::Allocate.", MR_LOG_LEVEL_ERROR);
            }
        } else {
            if(MR::MachineInfo::IsDirectStateAccessSupported()) {
                glNamedBufferDataEXT(_handle, _size, 0, usageFlags);
            } else {
                glBufferData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], _size, 0, usageFlags);
            }
        }*/
        if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            glNamedBufferDataEXT(_handle, _size, 0, usageFlags);
        } else {
            glBufferData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], _size, 0, usageFlags);
        }
        if(mapMemory) if(!MapMemory()) {
            MR::Log::LogString("Failed MapMemory in GPUBuffer::Allocate.", MR_LOG_LEVEL_ERROR);
        }
    )

    OnAllocated(dynamic_cast<MR::IGPUBuffer*>(this), usage, _size, mapMemory);
    if(binded) binded->Bind(ArrayBuffer);
}

bool GPUBuffer::BufferData(void* data, const size_t& offset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) {
    if(GetMappedMemory() != 0) return false;
    Assert(data == 0)
    Assert(size == 0)
    Assert(size+offset > GetGPUMem())
    Assert(GetGPUHandle() == 0)

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            glNamedBufferSubDataEXT(GetGPUHandle(), offset, size, data);
        } else {
            IGPUBuffer* binded = ReBind(ArrayBuffer);
            glBufferSubData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], offset, size, data);
            if(binded) binded->Bind(ArrayBuffer);
        }
    )

    if(out_realOffset) *out_realOffset = offset;
    if(out_info) *out_info = MR::IGPUBuffer::BufferedDataInfo(dynamic_cast<MR::IGPUBuffer*>(this), offset, size);
    return true;
}

bool GPUBuffer::MapMemory() {
    if(_mappingFlags == 0) {
        MR::Log::LogString("Failed GPUBuffer::MapMemory. Looks like buffer is not allocated.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    IGPUBuffer* binded = 0;
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        ///DAT STUPID glMapNamedBufferRange crash =(
        if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            if(_bindedTarget == NotBinded) {
                binded = ReBind(ArrayBuffer);
            }
        }
        _mapped_mem = glMapBufferRange(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)_bindedTarget], 0, _size, _mappingFlags);
    )
    if(binded) binded->Bind(ArrayBuffer);
    if(_mapped_mem) OnMemoryMapped(dynamic_cast<MR::IGPUBuffer*>(this), _mapped_mem);
    else return false;
    return true;
}

void GPUBuffer::UnMapMemory() {
    if(_mapped_mem == 0) return;
    bool bResult = true;
    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
            bResult = glUnmapNamedBufferEXT(_handle);
        )
    } else {
        IGPUBuffer* binded = 0;
        if(_bindedTarget == MR::IGPUBuffer::NotBinded) {
            binded = ReBind(ArrayBuffer);
        }
        MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
            bResult = glUnmapBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)_bindedTarget]);
        )
        if(binded) binded->Bind(ArrayBuffer);
    }
    _mapped_mem = 0;
    if(!bResult) {
        #if (MR_BUFFERS_CHECK_MAPPINGS_ERRORS == 1)
            MR::Log::LogString("Failed GPUBuffer::UnMap(). UnmapBuffer returned false.", MR_LOG_LEVEL_ERROR);
        #endif
    }
}

void GPUBuffer::Destroy() {
    if(_handle != 0) {
        if(GetMappedMemory() != 0) UnMapMemory();
        glDeleteBuffers(1, &_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<MR::GPUObjectHandle*>(this), 0);
        OnDestroy(dynamic_cast<MR::IGPUBuffer*>(this));
    }
}

GPUBuffer::GPUBuffer() : _mapped_mem(0), _bindedTarget(NotBinded), _size(0), _mappingFlags(0) {
}

GPUBuffer::~GPUBuffer() {
}

/** GLOBAL **/

IGPUBuffer* GPUBufferGetBinded(const IGPUBuffer::BindTargets& target) {
    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)

    return _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target];
}

void GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) {
    Assert(!src)
    Assert(!dst)
    Assert(srcOffset > src->GetGPUMem())
    Assert(dstOffset > dst->GetGPUMem())
    Assert(srcOffset+size > src->GetGPUMem())
    Assert(dstOffset+size > dst->GetGPUMem())

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glNamedCopyBufferSubDataEXT(src->GetGPUHandle(), dst->GetGPUHandle(), srcOffset, dstOffset, size);
        )
    } else {
        IGPUBuffer* binded_r = src->ReBind(IGPUBuffer::CopyReadBuffer);
        IGPUBuffer* binded_w = dst->ReBind(IGPUBuffer::CopyWriteBuffer);
        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);
        )
        if(binded_r) binded_r->Bind(IGPUBuffer::CopyReadBuffer);
        if(binded_w) binded_w->Bind(IGPUBuffer::CopyWriteBuffer);
    }
}

void GPUBufferUnBind(const IGPUBuffer::BindTargets& target) {
    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)
    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]) {
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]->Bind(IGPUBuffer::NotBinded);
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = nullptr;
    }
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
    glBindBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], 0);
    )
}

void GPUBufferUnBindAt(const IGPUBuffer::BindTargets& target, const unsigned int& index) {
    Assert(target < 0)
    Assert((size_t)target >= MR_BUFFERS_BIND_TARGETS_NUM)

    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]) {
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]->Bind(IGPUBuffer::NotBinded);
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = nullptr;
    }

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
    glBindBufferBase(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], index, 0);
    )
}

}
