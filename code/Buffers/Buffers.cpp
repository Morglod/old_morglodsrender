#include "Buffers.hpp"
#include "../Utils/Containers.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../StateCache.hpp"

#include <GL/glew.h>

/** GPUBuffer class implementation **/

namespace mr {

BufferRangeHdl::BufferRangeHdl(IBuffer* buf, size_t const& off, size_t const& sz) : _buffer(buf), _offset(off), _size(sz) {
}

BufferRangeHdl::~BufferRangeHdl() {
    if(!_disposed) {
        _disposed = true;
        _FreeRange();
    }
}


bool Buffer::Allocate(BufferUsage const& usage, const size_t& allocationSize) {
    AssertAndExec(allocationSize != 0, return false);

    if(GetGPUHandle() == 0) {
        unsigned int handle = 0;
        if(mr::gl::IsOpenGL45()) glCreateBuffers(1, &handle);
        else glGenBuffers(1, &handle);
        SetGPUHandle(handle);
    }

    if(_size > allocationSize) {
        mr::Log::LogString("Try GPUBuffer::Allocate, but already allocated for bigger allocationSize.", MR_LOG_LEVEL_WARNING);
        return true;
    }

    _size = allocationSize;
    _usage = usage;

    unsigned int usageFlags = 0;
    switch(usage) {
    case BufferUsage::Static:        usageFlags = GL_STATIC_DRAW;    break;
    case BufferUsage::FastChange:    usageFlags = GL_DYNAMIC_DRAW;   break;
    case BufferUsage::FastReadWrite: usageFlags = GL_STREAM_DRAW;    break;
    default:
        mr::Log::LogString("GPUBuffer::Allocate. Unknown usage option ("+std::to_string((int)usage)+"), Static will be used.", MR_LOG_LEVEL_WARNING);
        usageFlags = GL_STATIC_DRAW;
        break;
    }

    //Bind, because OnAllocated event may need this buffer rebind.
    IBuffer* binded = nullptr;
    StateCache* stateCache = nullptr;

    if(!mr::gl::IsDSA_EXT()) {
        stateCache = mr::StateCache::GetDefault();
        if(!stateCache->ReBindBuffer(dynamic_cast<IBuffer*>(this), StateCache::ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::Allocate.", MR_LOG_LEVEL_ERROR);
            return false;
        }
    }

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsDSA_ARB()) glNamedBufferData(GetGPUHandle(), _size, 0, usageFlags);
        else if(mr::gl::IsDSA_EXT()) glNamedBufferDataEXT(GetGPUHandle(), _size, 0, usageFlags);
        else glBufferData(GL_ARRAY_BUFFER, _size, 0, usageFlags);
        ,
        return false;
    )

    OnGPUBufferAllocated.Invoke(dynamic_cast<IBuffer*>(this), allocationSize);

    if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);

    return true;
}

bool Buffer::Write(void* __restrict__ srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* __restrict__ out_realOffset, BufferedDataInfo* __restrict__ out_info) {
    AssertAndExec(srcData != nullptr, return false);
    AssertAndExec(size != 0, return false);
    AssertAndExec(size+dstOffset <= GetGPUMem(), return false);
    AssertAndExec(GetGPUHandle() != 0, return false);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsDSA_ARB()) glNamedBufferSubData(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else if(mr::gl::IsDSA_EXT()) glNamedBufferSubDataEXT(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else {
            StateCache* stateCache = StateCache::GetDefault();
            IBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(dynamic_cast<IBuffer*>(this), StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GPUBuffer::Write.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            glBufferSubData(GL_ARRAY_BUFFER, dstOffset, size, (void*)((size_t)srcData+srcOffset));
            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
        ,
        return false;
    )

    if(out_realOffset) *out_realOffset = dstOffset;
    if(out_info) *out_info = mr::IBuffer::BufferedDataInfo(dynamic_cast<mr::IBuffer*>(this), dstOffset, size);
    return true;
}

bool Buffer::Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) {
    AssertAndExec(dstData != nullptr, return false);
    AssertAndExec(size+srcOffset < GetGPUMem(), return false);
    AssertAndExec(size != 0, return false);
    AssertAndExec(GetGPUHandle() != 0, return false);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsDSA_ARB())  glGetNamedBufferSubData(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else if(mr::gl::IsDSA_EXT()) glGetNamedBufferSubDataEXT(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else {
            StateCache* stateCache = StateCache::GetDefault();
            IBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(dynamic_cast<IBuffer*>(this), StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Bind buffer failed in GPUBuffer::Read.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            glGetBufferSubData(GL_ARRAY_BUFFER, srcOffset, size, (void*)((size_t)dstData+dstOffset));
            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
        ,
        return false;
    )

    return true;
}

IBuffer::IMappedRangePtr Buffer::Map(size_t const& offset, size_t const& length, unsigned int const& flags) {
    if(!_mapped.expired()) _mapped.lock()->UnMap();
    MappedRange* mapr = new MappedRange();
    IMappedRangePtr mrp = IMappedRangePtr(dynamic_cast<IMappedRange*>(mapr));
    if(!mapr->Map(this, offset, length, flags)) return nullptr;
    _mapped = IMappedRangeWPtr(mrp);
    return mrp;
}

IBufferRangeHdlWPtr Buffer::UseRange(size_t const& size, size_t const& offset) {
    IBufferRangeHdlPtr rangeHandlePtr = IBufferRangeHdlPtr(dynamic_cast<IBufferRangeHdl*>(new BufferRangeHdl(dynamic_cast<IBuffer*>(this), offset, size)));
    _rangeHandles.push_back(rangeHandlePtr);
    return IBufferRangeHdlWPtr(rangeHandlePtr);
}

mu::ArrayHandle<IBufferRangeHdl*> Buffer::GetRangeHandles() {
    IBufferRangeHdl** handles = new IBufferRangeHdl*[_rangeHandles.size()];
    for(size_t i = 0; i < _rangeHandles.size(); ++i) {
        handles[i] = _rangeHandles[i].get();
    }
    return mu::ArrayHandle<IBufferRangeHdl*>(handles, _rangeHandles.size(), true, false);
}

void Buffer::_RangeFree(IBufferRangeHdl* handle) {
    for(size_t i = 0; i < _rangeHandles.size(); ++i) {
        if(handle == _rangeHandles[i].get()) {
            _rangeHandles.erase(_rangeHandles.begin()+i);
            return;
        }
    }
}

void Buffer::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteBuffers(1, &handle);
        SetGPUHandle(0);
        _size = 0;
    }
}

Buffer::Buffer() : _size(0), _usage(BufferUsage::Static) {
}

Buffer::~Buffer() {
    for(size_t i = 0; i < _rangeHandles.size(); ++i) {
        _rangeHandles[i]->_StopHandle();
    }
    _rangeHandles.clear();
}

void Buffer::MappedRange::Flush() {
    if(_mem == 0) return;
    if(mr::gl::IsDSA_ARB()) glFlushMappedNamedBufferRange(_buffer->GetGPUHandle(), _offset, _length);
    else if(mr::gl::IsDSA_EXT()) glFlushMappedNamedBufferRangeEXT(_buffer->GetGPUHandle(), _offset, _length);
    else {
        StateCache* stateCache = StateCache::GetDefault();
        IBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(dynamic_cast<IBuffer*>(_buffer), StateCache::ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::Flush.", MR_LOG_LEVEL_ERROR);
            return;
        }
        glFlushMappedBufferRange(GL_ARRAY_BUFFER, _offset, _length);
        if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
    }
}

void Buffer::MappedRange::UnMap() {
    if(_mem == 0) return;
    if(mr::gl::IsDSA_ARB()) glUnmapNamedBuffer(_buffer->GetGPUHandle());
    else if(mr::gl::IsDSA_EXT()) glUnmapNamedBufferEXT(_buffer->GetGPUHandle());
    else {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        StateCache* stateCache = StateCache::GetDefault();
        IBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(dynamic_cast<IBuffer*>(_buffer), StateCache::ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::UnMap.", MR_LOG_LEVEL_ERROR);
            return;
        }

        glUnmapBuffer(GL_ARRAY_BUFFER);
        if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        ,
        mr::Log::LogString("Failed glUnmapBuffer in GPUBuffer::MappedRange::UnMap.", MR_LOG_LEVEL_WARNING);
    )
    }

    _mem = 0;
    _buffer = nullptr;
    _length = 0;
    _offset = 0;
}

bool Buffer::MappedRange::Map(Buffer* buf, size_t const& offset, size_t const& length, unsigned int const& flags) {
    AssertAndExec(buf != nullptr, return false);
    AssertAndExec(buf->GetGPUHandle() != 0, return false);
    //mr::Log::LogString(std::to_string(offset) + " " + std::to_string(length) + " " + std::to_string(buf->GetGPUMem()));
    AssertAndExec(offset + length <= buf->GetGPUMem(), return false);

    _offset = offset;
    _length = length;
    _buffer = buf;
    _flags = flags;

    if(mr::gl::IsDSA_ARB()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRange(buf->GetGPUHandle(), offset, length, flags);
        ,
        return false;
    )
    }
    else if(mr::gl::IsDSA_EXT()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRangeEXT(buf->GetGPUHandle(), offset, length, flags);
        ,
        return false;
    )
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        IBuffer* binded = nullptr;
        if(!stateCache->ReBindBuffer(buf, StateCache::ArrayBuffer, &binded)) {
            mr::Log::LogString("Bind buffer failed in GPUBuffer::MappedRange::Map.", MR_LOG_LEVEL_ERROR);
            return false;
        }
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapBufferRange(GL_ARRAY_BUFFER, offset, length, flags);
        ,
        mr::Log::LogString("Failed glMapBufferRange in GPUBuffer::MappedRange::Map.", MR_LOG_LEVEL_ERROR);
        return false;
    )
        if(binded) {
            stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
    }
    if(_mem == 0) {
        mr::Log::LogString("Failed glMapBufferRange in GPUBuffer::MappedRange::Map. Mapped mem is zero.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    return true;
}

Buffer::MappedRange::MappedRange() : _buffer(nullptr), _mem(0), _length(0) {}

Buffer::MappedRange::~MappedRange() {
    UnMap();
}

bool Buffer::MakeResident() {
    const unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed GPUBuffer::MakeResident. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(mr::gl::IsNVVBUMSupported()) {
        if(mr::gl::IsDSA_ARB()) {
            glGetNamedBufferParameterui64vNV(handle, GL_BUFFER_GPU_ADDRESS_NV, &_residentPtr);
            glMakeNamedBufferResidentNV(handle, GL_READ_ONLY);
        } else {
            StateCache* stateCache = StateCache::GetDefault();
            IBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(this, StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Failed GPUBuffer::MakeResident. Bind buffer failed.", MR_LOG_LEVEL_ERROR);
                return false;
            }

            glGetBufferParameterui64vNV(GL_ARRAY_BUFFER, GL_BUFFER_GPU_ADDRESS_NV, &_residentPtr);
            glMakeBufferResidentNV(GL_ARRAY_BUFFER, GL_READ_ONLY);

            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
        return true;
    }
    return false;
}

void Buffer::MakeNonResident() {
    const unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed GPUBuffer::MakeResident. Handle is null.", MR_LOG_LEVEL_ERROR);
        return;
    }
    if(mr::gl::IsNVVBUMSupported()) {
        if(mr::gl::IsDSA_ARB()) {
            glMakeNamedBufferNonResidentNV(handle);
        } else {
            StateCache* stateCache = StateCache::GetDefault();
            IBuffer* binded = nullptr;
            if(!stateCache->ReBindBuffer(this, StateCache::ArrayBuffer, &binded)) {
                mr::Log::LogString("Failed GPUBuffer::MakeResident. Bind buffer failed.", MR_LOG_LEVEL_ERROR);
                return;
            }

            glMakeBufferNonResidentNV(GL_ARRAY_BUFFER);

            if(binded) stateCache->BindBuffer(binded, StateCache::ArrayBuffer);
        }
    }
}

/** GLOBAL **/

bool GPUBufferCopy(IBuffer* src, IBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) {
    Assert(src != nullptr);
    Assert(dst != nullptr);
    Assert(srcOffset < src->GetGPUMem());
    Assert(dstOffset < dst->GetGPUMem());
    Assert(srcOffset+size < src->GetGPUMem());
    Assert(dstOffset+size < dst->GetGPUMem());

    if(mr::gl::IsDSA_EXT()) {
        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glNamedCopyBufferSubDataEXT(src->GetGPUHandle(), dst->GetGPUHandle(), srcOffset, dstOffset, size);
            ,
            return false;
        )
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        IBuffer* binded_r = nullptr;
        IBuffer* binded_w = nullptr;

        if(!stateCache->ReBindBuffer(src, StateCache::CopyReadBuffer, &binded_r)) {
            mr::Log::LogString("Bind src buffer failed in GPUBufferCopy.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        if(!stateCache->ReBindBuffer(dst, StateCache::CopyWriteBuffer, &binded_w)) {
            mr::Log::LogString("Bind dst buffer failed in GPUBufferCopy.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
            glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, srcOffset, dstOffset, size);
            ,
            return false;
        )
        if(binded_r) stateCache->BindBuffer(binded_r, StateCache::CopyReadBuffer);
        if(binded_w) stateCache->BindBuffer(binded_w, StateCache::CopyWriteBuffer);
    }
    return true;
}

}
