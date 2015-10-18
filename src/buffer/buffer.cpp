#include "mr/buffer/buffer.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"

#include "src/thread/util.hpp"

namespace {

struct _BufferInfo {
    uint32_t buffer;
    int32_t access; //mapBuffer
    int32_t access_flags; //mapBufferRange
    int32_t immutable_storage; // 0 or 1
    int32_t mapped;
    int32_t map_length;
    int32_t map_offset;
    int32_t size;
    int32_t storage_flags;
    int32_t usage;
    void* mapped_mem;
};

void _GetBufferInfo(uint32_t buffer, _BufferInfo& info) {
    info.buffer = buffer;
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_ACCESS, &info.access);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_ACCESS_FLAGS, &info.access_flags);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_IMMUTABLE_STORAGE, &info.immutable_storage);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_MAPPED, &info.mapped);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_MAP_LENGTH, &info.map_length);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_MAP_OFFSET, &info.map_offset);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_SIZE, &info.size);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_STORAGE_FLAGS, &info.storage_flags);
    glGetNamedBufferParameteriv(buffer, GL_BUFFER_USAGE, &info.usage);
    glGetNamedBufferPointerv(buffer, GL_BUFFER_MAP_POINTER, &info.mapped_mem);
}

}

namespace mr {

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
    glNamedBufferStorage(buffer, size, mem->GetPtr(), flags_i);
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

Buffer::MappedMem Buffer::Map(uint32_t length, Buffer::MapOptFlags const& flags, uint32_t offset) {
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
        } else return _mapState;
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
    if(!IsMapped()) return true;
    if(!glUnmapNamedBuffer(_id))
        MR_LOG_ERROR(Buffer::UnMap, "Failed unmap buffer");
    return !IsMapped();
}

bool Buffer::MakeResident(bool read, bool write) {
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
        glGetNamedBufferParameterui64vNV(_id, GL_BUFFER_GPU_ADDRESS_NV, &_resident.address);
        return true;
    } else return false;
}

bool Buffer::MakeNonResident() {
    if(!GLEW_NV_shader_buffer_load) {
        MR_LOG_ERROR(Buffer::MakeNonResident, "NV_shader_buffer_load is not supported");
        return false;
    }
    glMakeNamedBufferNonResidentNV(_id);
    return !glIsNamedBufferResidentNV(_id);
}

std::future<bool> Buffer::Write(MemoryPtr const& mem_src, uint32_t offset) {
    std::promise<bool> promise;
    auto futur = promise.get_future();

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
            memcpy(dst, mem->GetPtr(), mem->GetSize());
            return true;
        }, mem_src, _mapState.mem);
}

std::future<bool> Buffer::Read(MemoryPtr const& mem_dst, uint32_t offset) {
    std::promise<bool> promise;
    auto futur = promise.get_future();

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
            memcpy(mem->GetPtr(), src, mem->GetSize());
            return true;
        }, mem_dst, _mapState.mem);
}

Buffer::Buffer() : _id(0), _size(0), _mapState(), _resident() {
}

Buffer::~Buffer() {
}

}
