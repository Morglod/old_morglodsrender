#include "mr/buffer.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"
#include "src/mp.hpp"
#include "src/thread/util.hpp"

namespace mr {

void BufferInfo::Get(uint32_t buf) {
    MP_BeginSample(BufferInfo::Get);

    buffer = buf;
    glGetNamedBufferParameteriv(buf, GL_BUFFER_ACCESS, &access);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_ACCESS_FLAGS, &access_flags);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_IMMUTABLE_STORAGE, &immutable_storage);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_MAPPED, &mapped);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_MAP_LENGTH, &map_length);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_MAP_OFFSET, &map_offset);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_SIZE, &size);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_STORAGE_FLAGS, &storage_flags);
    glGetNamedBufferParameteriv(buf, GL_BUFFER_USAGE, &usage);
    glGetNamedBufferPointerv(buf, GL_BUFFER_MAP_POINTER, &mapped_mem);

    MP_EndSample();
}

Buffer::MapFlags::MapFlags(Buffer::MapFlags const& cpy) {
    read = cpy.read;
    write = cpy.write;
    persistent = cpy.persistent;
    coherent = cpy.coherent;
}

Buffer::MapOptFlags::MapOptFlags(Buffer::MapFlags const& cpy) {
    read = cpy.read;
    write = cpy.write;
    persistent = cpy.persistent;
    coherent = cpy.coherent;
}

BufferPtr Buffer::Create(MemoryPtr const& mem, CreationFlags const& flags) {
    MP_ScopeSample(Buffer::Create);

    uint32_t flags_i =  (flags.client_storage ? GL_CLIENT_STORAGE_BIT : 0) |
                        (flags.coherent ? GL_MAP_COHERENT_BIT : 0) |
                        (flags.dynamic ? GL_DYNAMIC_STORAGE_BIT : 0) |
                        (flags.persistent ? GL_MAP_PERSISTENT_BIT : 0) |
                        (flags.read ? GL_MAP_READ_BIT : 0) |
                        (flags.write ? GL_MAP_WRITE_BIT : 0);

    BufferPtr buf = BufferPtr(new Buffer());
    uint32_t buffer = 0;
    glCreateBuffers(1, &buffer);
    const auto size = mem->GetSize();
    //glNamedBufferStorage(buffer, size, mem->GetPtr(), flags_i);
    glNamedBufferData(buffer, size, mem->GetPtr(), GL_STATIC_DRAW);
    buf->_id = buffer;
    buf->_size = size;
    buf->_mapState.mem = nullptr;

    if(flags.map_after_creation) {
        buf->Map(size, MapOptFlags(static_cast<MapFlags>(flags)), 0);
        if(!buf->IsMapped()) {
            MR_LOG_ERROR(Buffer::Create, "Failed map buffer after creation");
            return nullptr;
        }
    }
    return buf;
}

void Buffer::Destroy() {
    MP_ScopeSample(Buffer::Destroy);

    if(_id == 0) return;
    glDeleteBuffers(1, &_id);
    _id = 0;
}

Buffer::MappedMem Buffer::Map(uint32_t length, Buffer::MapOptFlags const& flags, uint32_t offset) {
    MP_ScopeSample(Buffer::Map);

    uint32_t flags_i =  (flags.coherent ? GL_MAP_COHERENT_BIT : 0) |
                        (flags.persistent ? GL_MAP_PERSISTENT_BIT : 0) |
                        (flags.read ? GL_MAP_READ_BIT : 0) |
                        (flags.write ? GL_MAP_WRITE_BIT : 0) |
                        // optional
                        (flags.invalidate_range ? GL_MAP_INVALIDATE_RANGE_BIT : 0) |
                        (flags.invalidate_buffer ? GL_MAP_INVALIDATE_BUFFER_BIT : 0) |
                        (flags.flush_explicit ? GL_MAP_FLUSH_EXPLICIT_BIT : 0) |
                        (flags.unsynchronized ? GL_MAP_UNSYNCHRONIZED_BIT : 0);

    // Sizes error
    if((length + offset) > (uint32_t)_size) {
        MR_LOG_ERROR(Buffer::Map, "length + offset > buffer.size");
        return _mapState;
    }

    // Need remap ?
    if(IsMapped()) {
        if((_mapState.offset > offset) || (_mapState.flags != flags_i)) {
            if(!UnMap()) {
                MR_LOG_ERROR(Buffer::Map, "Failed unmap buffer, before remap");
                return _mapState;
            }
        } else {
            return _mapState;
        }
    }

    _mapState.mem = glMapNamedBufferRange(_id, offset, length, flags_i);
    _mapState.offset = offset;
    _mapState.length = length;
    _mapState.flags = flags_i;

    if(_mapState.mem == nullptr) {
        MR_LOG_ERROR(Buffer::Map, "Failed map buffer");
        MR_LOG_T_STD_("buffer.length: ", length);
    }

    return _mapState;
}

bool Buffer::UnMap() {
    MP_ScopeSample(Buffer::UnMap);

    if(!IsMapped()) {
        return true;
    }
    if(!glUnmapNamedBuffer(_id))
        MR_LOG_ERROR(Buffer::UnMap, "Failed unmap buffer");

    return !IsMapped();
}

void Buffer::FlushMapped() {
    MP_ScopeSample(Buffer::FlushMapped);

    if(!IsMapped()) {
        MR_LOG_WARNING(Buffer::FlushMapped, "connot flush unmapped buffer");
        return;
    }

    if(!(_mapState.flags & GL_MAP_FLUSH_EXPLICIT_BIT)) {
        MR_LOG_ERROR(Buffer::FlushMapped, "failed flush buffer, without flush_explicit flag");
        return;
    }

    glFlushMappedNamedBufferRange(_id, _mapState.offset, _mapState.length);
}

bool Buffer::MakeResident(bool read, bool write) {
    MP_ScopeSample(Buffer::MakeResident);

    if(!GLEW_NV_shader_buffer_load) {
        MR_LOG_ERROR(Buffer::MakeResident, "NV_shader_buffer_load is not supported");
        return false;
    }

    if(IsResident())
        if(_resident.read != read || _resident.write != write)
            if(!MakeNonResident()) {
                return false;
            }

    GLenum access_flag = GL_READ_ONLY;
    if(read && write) access_flag = GL_READ_WRITE;
    else if(write) access_flag = GL_WRITE_ONLY;
    else if(read) access_flag = GL_READ_ONLY;
    else return false;

    glMakeNamedBufferResidentNV(_id, access_flag);
    if(glIsNamedBufferResidentNV(_id)) {
        _resident.resident = true;
        glGetNamedBufferParameterui64vNV(_id, GL_BUFFER_GPU_ADDRESS_NV, &_resident.address);
        return true;
    } else return false;
}

bool Buffer::MakeNonResident() {
    MP_ScopeSample(Buffer::MakeNonResident);

    if(!GLEW_NV_shader_buffer_load) {
        MR_LOG_ERROR(Buffer::MakeNonResident, "NV_shader_buffer_load is not supported");
        return false;
    }
    glMakeNamedBufferNonResidentNV(_id);
    const bool isResident = glIsNamedBufferResidentNV(_id);
    _resident.resident = isResident;
    return !isResident;
}

std::future<bool> Buffer::WriteAsync(MemoryPtr const& mem_src, uint32_t offset) {
    MP_ScopeSample(Buffer::WriteAsync);

    std::promise<bool> promise;
    auto futur = promise.get_future();

    //Assert
    if((offset + mem_src->GetSize()) > ((uint32_t)GetSize())) {
        MR_LOG_ERROR(Buffer::WriteAsync, "Failed write data: offset + mem.size > buffer.size");
        promise.set_value(false);
        return futur;
    }

    // Should be mapped
    MapFlags flags;
    if(IsMapped()) {
        flags.coherent = _mapState.flags & GL_MAP_COHERENT_BIT;
        flags.persistent = _mapState.flags & GL_MAP_PERSISTENT_BIT;
        flags.read = _mapState.flags & GL_MAP_READ_BIT;
    }
    flags.write = true;

    Map(mem_src->GetSize(), flags, offset);
    if(!IsMapped()) {
        MR_LOG_ERROR(Buffer::Write, "Failed map memory");
        promise.set_value(false);
        return futur;
    }

    // Run write task
    return std::async(std::launch::async,
        [](MemoryPtr const& mem, void* dst) -> bool {
            MP_ScopeSample(Buffer::Write::memcpy);
            memcpy(dst, mem->GetPtr(), mem->GetSize());
            return true;
        }, mem_src, _mapState.mem);
}

std::future<bool> Buffer::ReadAsync(MemoryPtr const& mem_dst, uint32_t offset) {
    MP_ScopeSample(Buffer::ReadAsync);

    std::promise<bool> promise;
    auto futur = promise.get_future();

    //Assert
    if((offset + mem_dst->GetSize()) > (uint32_t)GetSize()) {
        MR_LOG_ERROR(Buffer::ReadAsync, "Failed read data: offset + mem.size > buffer.size");
        promise.set_value(false);
        return futur;
    }

    // Should be mapped
    MapFlags flags;
    if(IsMapped()) {
        flags.coherent = _mapState.flags & GL_MAP_COHERENT_BIT;
        flags.persistent = _mapState.flags & GL_MAP_PERSISTENT_BIT;
        flags.write = _mapState.flags & GL_MAP_WRITE_BIT;
    }
    flags.read = true;

    Map(mem_dst->GetSize(), flags, offset);
    if(!IsMapped()) {
        MR_LOG_ERROR(Buffer::Write, "Failed map memory");
        promise.set_value(false);
        return futur;
    }

    // Run write task
    return std::async(std::launch::async,
        [](MemoryPtr const& mem, void* src) -> bool {
            MP_ScopeSample(Buffer::Read::memcpy);
            memcpy(mem->GetPtr(), src, mem->GetSize());
            return true;
        }, mem_dst, _mapState.mem);
}

bool Buffer::Write(MemoryPtr const& mem_src, uint32_t offset) {
    MP_ScopeSample(Buffer::Write);

    //Assert
    if((offset + mem_src->GetSize()) > (uint32_t)GetSize()) {
        MR_LOG_ERROR(Buffer::Write, "Failed write data: offset + mem.size > buffer.size");
        return false;
    }

    glNamedBufferSubData(GetId(), offset, mem_src->GetSize(), mem_src->GetPtr());

    return true;
}

bool Buffer::Read(MemoryPtr const& mem_dst, uint32_t offset) {
    MP_ScopeSample(Buffer::Read);

    //Assert
    if((offset + mem_dst->GetSize()) > (uint32_t)GetSize()) {
        MR_LOG_ERROR(Buffer::Write, "Failed read data: offset + mem.size > buffer.size");
        return false;
    }

    glGetNamedBufferSubData(GetId(), offset, mem_dst->GetSize(), mem_dst->GetPtr());

    return true;
}

Buffer::Buffer() : _id(0), _size(0), _mapState(), _resident() {
}

Buffer::~Buffer() {
    Destroy();
}

}
