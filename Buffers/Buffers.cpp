#include "Buffers.hpp"
#include "../Utils/Containers.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#define MR_BUFFERS_BIND_TARGETS_NUM 14
mr::TStaticArray<mr::IGPUBuffer*> _MR_BUFFERS_BIND_TARGETS_(new mr::IGPUBuffer*[MR_BUFFERS_BIND_TARGETS_NUM], MR_BUFFERS_BIND_TARGETS_NUM, true);

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

mr::TDynamicArray<mr::IGPUBuffer*> _MR_REGISTERED_BUFFERS_;

/** GPUBuffer class implementation **/

namespace mr {

void GPUBuffer::Bind(const IGPUBuffer::BindTarget& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);
    Assert(GetGPUHandle() != 0);

    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] == dynamic_cast<mr::IGPUBuffer*>(this)) return;
    _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = dynamic_cast<mr::IGPUBuffer*>(this);
    _bindedTarget = target;

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle());
    )
}

void GPUBuffer::BindAt(const BindTarget& target, const unsigned int& index) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);
    Assert(GetGPUHandle() != 0);

    _bindedTarget = target;

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle(), index);
    )
}

IGPUBuffer* GPUBuffer::ReBind(const IGPUBuffer::BindTarget& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return 0;
    }

    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);

    IGPUBuffer* binded = _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target];
    Bind(target);
    return binded;
}

IGPUBuffer::BindTarget GPUBuffer::GetBindTarget() {
    return _bindedTarget;
}

void GPUBuffer::Allocate(const Usage& usage, const size_t& size) {
    Assert(size != 0);
    if(_handle == 0) {
        if(mr::gl::IsOpenGL45()) glCreateBuffers(1, &_handle);
        else glGenBuffers(1, &_handle);
        OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), _handle);
    }

    if(_size > size) return; //Current size is enough
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
    IGPUBuffer* binded = 0;
    if(!mr::gl::IsDirectStateAccessSupported()) binded = ReBind(ArrayBuffer);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45()) glNamedBufferData(_handle, _size, 0, usageFlags);
        else if(mr::gl::IsDirectStateAccessSupported()) glNamedBufferDataEXT(_handle, _size, 0, usageFlags);
        else glBufferData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], _size, 0, usageFlags);
    )

    OnAllocated(dynamic_cast<mr::IGPUBuffer*>(this), usage, _size);
    if(binded) binded->Bind(ArrayBuffer);
}

bool GPUBuffer::Write(void* srcData, const size_t& srcOffset, const size_t& dstOffset, const size_t& size, size_t* out_realOffset, BufferedDataInfo* out_info) {
    Assert(srcData != nullptr);
    Assert(size != 0);
    Assert(size+dstOffset <= GetGPUMem());
    Assert(GetGPUHandle() != 0);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45()) glNamedBufferSubData(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else if(mr::gl::IsDirectStateAccessSupported()) glNamedBufferSubDataEXT(GetGPUHandle(), dstOffset, size, (void*)((size_t)srcData+srcOffset));
        else {
            IGPUBuffer* binded = ReBind(ArrayBuffer);
            glBufferSubData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], dstOffset, size, (void*)((size_t)srcData+srcOffset));
            if(binded) binded->Bind(ArrayBuffer);
        }
    )

    if(out_realOffset) *out_realOffset = dstOffset;
    if(out_info) *out_info = mr::IGPUBuffer::BufferedDataInfo(dynamic_cast<mr::IGPUBuffer*>(this), dstOffset, size);
    return true;
}

bool GPUBuffer::Read(void* dstData, const size_t& dstOffset, const size_t& srcOffset, const size_t& size) {
    Assert(dstData != nullptr);
    Assert(size+srcOffset < GetGPUMem());
    Assert(size != 0);
    Assert(GetGPUHandle() != 0);

    MR_BUFFERS_CHECK_BUFFER_DATA_ERRORS_CATCH(
        if(mr::gl::IsOpenGL45())  glGetNamedBufferSubData(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else if(mr::gl::IsDirectStateAccessSupported()) glGetNamedBufferSubDataEXT(GetGPUHandle(), srcOffset, size, (void*)((size_t)dstData+dstOffset));
        else {
            IGPUBuffer* binded = ReBind(ArrayBuffer);
            glGetBufferSubData(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], srcOffset, size, (void*)((size_t)dstData+dstOffset));
            if(binded) binded->Bind(ArrayBuffer);
        }
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

GPUBuffer::GPUBuffer() : _bindedTarget(NotBinded), _size(0), _usage(Static) {
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
        IGPUBuffer* binded = _buffer->ReBind(ArrayBuffer);
        glFlushMappedBufferRange(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], _offset, _length);
        if(binded) binded->Bind(ArrayBuffer);
    }
}

void GPUBuffer::MappedRange::UnMap() {
    if(_mem == 0) return;
    if(mr::gl::IsOpenGL45()) glUnmapNamedBuffer(_buffer->GetGPUHandle());
    else if(mr::gl::IsDirectStateAccessSupported()) glUnmapNamedBufferEXT(_buffer->GetGPUHandle());
    else {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        IGPUBuffer* binded = _buffer->ReBind(ArrayBuffer);
        glUnmapBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer]);
        if(binded) binded->Bind(ArrayBuffer);
    )
    }
    _mem = 0;
    _buffer = nullptr;
    _length = 0;
    _offset = 0;
}

bool GPUBuffer::MappedRange::Map(GPUBuffer* buf, size_t const& offset, size_t const& length, unsigned int const& flags) {
    Assert(buf != nullptr);
    Assert(buf->GetGPUHandle() != 0);
    Assert(offset + length <= buf->GetGPUMem());

    _offset = offset;
    _length = length;
    _buffer = buf;
    _flags = flags;

    if(mr::gl::IsOpenGL45()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRange(buf->GetGPUHandle(), offset, length, flags);
    )
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapNamedBufferRangeEXT(buf->GetGPUHandle(), offset, length, flags);
    )
    }
    else {
        IGPUBuffer* binded = buf->ReBind(ArrayBuffer);
    MR_BUFFERS_CHECK_MAPPINGS_ERRORS_CATCH(
        _mem = (char*)glMapBufferRange(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)ArrayBuffer], offset, length, flags);
    )
        if(binded) binded->Bind(ArrayBuffer);
    }
    if(_mem == 0) return false;
    return true;
}

GPUBuffer::MappedRange::MappedRange() : _buffer(nullptr), _mem(0), _length(0) {}

GPUBuffer::MappedRange::~MappedRange() {
    UnMap();
}

/** GLOBAL **/

IGPUBuffer* GPUBufferGetBinded(const IGPUBuffer::BindTarget& target) {
    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);
    return _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target];
}

void GPUBufferCopy(IGPUBuffer* src, IGPUBuffer* dst, const unsigned int& srcOffset, const unsigned int& dstOffset, const unsigned int& size) {
    Assert(src != nullptr);
    Assert(dst != nullptr);
    Assert(srcOffset < src->GetGPUMem());
    Assert(dstOffset < dst->GetGPUMem());
    Assert(srcOffset+size < src->GetGPUMem());
    Assert(dstOffset+size < dst->GetGPUMem());

    if(mr::gl::IsDirectStateAccessSupported()) {
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

void GPUBufferUnBind(const IGPUBuffer::BindTarget& target) {
    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);
    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]) {
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]->Bind(IGPUBuffer::NotBinded);
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = nullptr;
    }
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBuffer(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], 0);
    )
}

void GPUBufferUnBindAt(const IGPUBuffer::BindTarget& target, const unsigned int& index) {
    Assert((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM);

    if(_MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]) {
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target]->Bind(IGPUBuffer::NotBinded);
        _MR_BUFFERS_BIND_TARGETS_.GetRaw()[(size_t)target] = nullptr;
    }

    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
    glBindBufferBase(_MR_BUFFER_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], index, 0);
    )
}

void DestroyAllBuffers() {
    for(size_t i = 0; i < _MR_REGISTERED_BUFFERS_.GetNum(); ++i) {
        _MR_REGISTERED_BUFFERS_.At(i)->Destroy();
    }
}

}
