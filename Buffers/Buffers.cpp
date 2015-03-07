#include "Buffers.hpp"
#include "../Utils/Containers.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../StateCache.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::TDynamicArray<mr::IGPUBuffer*> _MR_REGISTERED_BUFFERS_;

/** GPUBuffer class implementation **/

namespace mr {

bool GPUBuffer::Allocate(const Usage& usage, const size_t& size) {
    AssertAndExec(size != 0, return false);

    if(_handle == 0) {
        if(mr::gl::IsOpenGL45()) glCreateBuffers(1, &_handle);
        else glGenBuffers(1, &_handle);
        OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), _handle);
    }

    if(_size > size) {
        mr::Log::LogString("Try GPUBuffer::Allocate, but already allocated for bigger size.", MR_LOG_LEVEL_WARNING);
        return true;
    }

    _size = size;
    _usage = usage;
	
    unsigned int usageFlags = 0;
    switch(usage) {
    case Static:
        usageFlags = GL_STATIC_DRAW;
        break;
    case FastChange:
        usageFlags = GL_STREAM_DRAW;
        break;
    default:
        mr::Log::LogString("GPUBuffer::Allocate. Unknown usage option ("+std::to_string((int)usage)+"), Static will be used.", MR_LOG_LEVEL_WARNING);
        usageFlags = GL_STATIC_DRAW;
        break;
    }

    //Bind, because OnAllocated event may need this buffer rebind.
    IGPUBuffer* binded = nullptr;
    StateCache* stateCache = nullptr;

    if(!mr::gl::IsDirectStateAccessSupported()) {
        stateCache = mr::StateCache::GetDefault();
        if(!stateCache->ReBindBuffer(dynamic_cast<IGPUBuffer*>(this), ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::Allocate.", MR_LOG_LEVEL_ERROR);
            return false;
        }
    }

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45()) glNamedBufferData(_handle, _size, 0, usageFlags);
        else if(mr::gl::IsDirectStateAccessSupported()) glNamedBufferDataEXT(_handle, _size, 0, usageFlags);
        else glBufferData(GL_ARRAY_BUFFER, _size, 0, usageFlags);
        ,
        return false;
    )

    OnAllocated(dynamic_cast<mr::IGPUBuffer*>(this), usage, _size);
    if(binded) stateCache->BindBuffer(binded, ArrayBuffer);

    return true;
}

bool GPUBuffer::Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) {
    AssertAndExec(srcData != nullptr, return false);
    AssertAndExec(size != 0, return false);
    AssertAndExec(size+dstOffset <= GetGPUMem(), return false);
    AssertAndExec(GetGPUHandle() != 0, return false);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45()) glNamedBufferSubData(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else if(mr::gl::IsDirectStateAccessSupported()) glNamedBufferSubDataEXT(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else {
            StateCache* stateCache = StateCache::GetDefault();
            IGPUBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(dynamic_cast<IGPUBuffer*>(this), ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GPUBuffer::Write.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            glBufferSubData(GL_ARRAY_BUFFER, dstOffset, size, (void*)((size_t)srcData+srcOffset));
            if(binded) stateCache->BindBuffer(binded, ArrayBuffer);
        }
        ,
        return false;
    )

    if(out_realOffset) *out_realOffset = dstOffset;
    if(out_info) *out_info = mr::IGPUBuffer::BufferedDataInfo(dynamic_cast<mr::IGPUBuffer*>(this), dstOffset, size);
    return true;
}

bool GPUBuffer::Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) {
    AssertAndExec(dstData != nullptr, return false);
    AssertAndExec(size+srcOffset < GetGPUMem(), return false);
    AssertAndExec(size != 0, return false);
    AssertAndExec(GetGPUHandle() != 0, return false);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45())  glGetNamedBufferSubData(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else if(mr::gl::IsDirectStateAccessSupported()) glGetNamedBufferSubDataEXT(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else {
            StateCache* stateCache = StateCache::GetDefault();
            IGPUBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(dynamic_cast<IGPUBuffer*>(this), ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GPUBuffer::Read.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            glGetBufferSubData(GL_ARRAY_BUFFER, srcOffset, size, (void*)((size_t)dstData+dstOffset));
            if(binded) stateCache->BindBuffer(binded, ArrayBuffer);
        }
        ,
        return false;
    )

    return true;
}

IGPUBuffer::IMappedRangePtr GPUBuffer::Map(size_t const& offset, size_t const& length, unsigned int const& flags) {
    if(!_mapped.expired()) _mapped.lock()->UnMap();
    MappedRange* mapr = new MappedRange();
    IMappedRangePtr mrp = IMappedRangePtr(dynamic_cast<IMappedRange*>(mapr));
    if(!mapr->Map(this, offset, length, flags)) return nullptr;
    _mapped = IMappedRangeWeakPtr(mrp);
    return mrp;
}

void GPUBuffer::Destroy() {
    if(_handle != 0) {
        glDeleteBuffers(1, &_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
    }
}

GPUBuffer::GPUBuffer() : _size(0), _usage(Static) {
    _MR_REGISTERED_BUFFERS_.PushBack(dynamic_cast<mr::IGPUBuffer*>(this));
}

GPUBuffer::~GPUBuffer() {
    _MR_REGISTERED_BUFFERS_.Erase(dynamic_cast<mr::IGPUBuffer*>(this));
}

void GPUBuffer::MappedRange::Flush() {
    if(_mem == 0) return;
    if(mr::gl::IsOpenGL45()) glFlushMappedNamedBufferRange(_buffer->GetGPUHandle(), _offset, _length);
    else if(mr::gl::IsDirectStateAccessSupported()) glFlushMappedNamedBufferRangeEXT(_buffer->GetGPUHandle(), _offset, _length);
    else {
        StateCache* stateCache = StateCache::GetDefault();
        IGPUBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(dynamic_cast<IGPUBuffer*>(_buffer), ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::Flush.", MR_LOG_LEVEL_ERROR);
            return;
        }
        glFlushMappedBufferRange(GL_ARRAY_BUFFER, _offset, _length);
        if(binded) stateCache->BindBuffer(binded, ArrayBuffer);
    }
}

void GPUBuffer::MappedRange::UnMap() {
    if(_mem == 0) return;
    if(mr::gl::IsOpenGL45()) glUnmapNamedBuffer(_buffer->GetGPUHandle());
    else if(mr::gl::IsDirectStateAccessSupported()) glUnmapNamedBufferEXT(_buffer->GetGPUHandle());
    else {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        StateCache* stateCache = StateCache::GetDefault();
        IGPUBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(dynamic_cast<IGPUBuffer*>(_buffer), ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::UnMap.", MR_LOG_LEVEL_ERROR);
            return;
        }

        glUnmapBuffer(GL_ARRAY_BUFFER);
        if(binded) stateCache->BindBuffer(binded, ArrayBuffer);
        ,
        mr::Log::LogString("Failed glUnmapBuffer in GPUBuffer::MappedRange::UnMap.", MR_LOG_LEVEL_WARNING);
    )
    }
	
    _mem = 0;
    _buffer = nullptr;
    _length = 0;
    _offset = 0;
}

bool GPUBuffer::MappedRange::Map(GPUBuffer* buf, size_t const& offset, size_t const& length, unsigned int const& flags) {
    AssertAndExec(buf != nullptr, return false);
    AssertAndExec(buf->GetGPUHandle() != 0, return false);
    AssertAndExec(offset + length <= buf->GetGPUMem(), return false);

    _offset = offset;
    _length = length;
    _buffer = buf;
    _flags = flags;

    if(mr::gl::IsOpenGL45()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRange(buf->GetGPUHandle(), offset, length, flags);
        ,
        return false;
    )
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRangeEXT(buf->GetGPUHandle(), offset, length, flags);
        ,
        return false;
    )
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        IGPUBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(buf, ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::Map.", MR_LOG_LEVEL_ERROR);
            return false;
        }
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapBufferRange(GL_ARRAY_BUFFER, offset, length, flags);
        ,
        mr::Log::LogString("Failed glMapBufferRange in GPUBuffer::MappedRange::Map.", MR_LOG_LEVEL_WARNING);
        return false;
    )
        if(binded) {
            stateCache->BindBuffer(binded, ArrayBuffer);
        }
    }
    if(_mem == 0) return false;
    return true;
}

GPUBuffer::MappedRange::MappedRange() : _buffer(nullptr), _mem(0), _length(0) {}

GPUBuffer::MappedRange::~MappedRange() {
    UnMap();
}

/** GLOBAL **/

bool GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) {
    Assert(src != nullptr);
    Assert(dst != nullptr);
    Assert(srcOffset < src->GetGPUMem());
    Assert(dstOffset < dst->GetGPUMem());
    Assert(srcOffset+size < src->GetGPUMem());
    Assert(dstOffset+size < dst->GetGPUMem());

    if(mr::gl::IsDirectStateAccessSupported()) {
        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glNamedCopyBufferSubDataEXT(src->GetGPUHandle(), dst->GetGPUHandle(), srcOffset, dstOffset, size);
            ,
            return false;
        )
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        IGPUBuffer* binded_r = nullptr;
        IGPUBuffer* binded_w = nullptr;

        if(!stateCache->ReBindBuffer(src, IGPUBuffer::CopyReadBuffer, &binded_r)) {
            mr::Log::LogString("Bind src buffer failed in GPUBufferCopy.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        if(!stateCache->ReBindBuffer(dst, IGPUBuffer::CopyWriteBuffer, &binded_w)) {
            mr::Log::LogString("Bind dst buffer failed in GPUBufferCopy.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);
            ,
            return false;
        )
        if(binded_r) stateCache->BindBuffer(binded_r, IGPUBuffer::CopyReadBuffer);
        if(binded_w) stateCache->BindBuffer(binded_w, IGPUBuffer::CopyWriteBuffer);
    }
    return true;
}

void DestroyAllBuffers() {
    for(size_t i = 0; i < _MR_REGISTERED_BUFFERS_.GetNum(); ++i) {
        _MR_REGISTERED_BUFFERS_.At(i)->Destroy();
    }
}

}
