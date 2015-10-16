#include "mr/buffer/buffer.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"
#include "mr/core.hpp"

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

std::future<BufferPtr> Buffer::Create(MemoryPtr const& mem, CreationFlags const& flags) {
    uint32_t flags_i =  (flags.client_storage ? GL_CLIENT_STORAGE_BIT : 0) |
                        (flags.coherent ? GL_MAP_COHERENT_BIT : 0) |
                        (flags.dynamic ? GL_DYNAMIC_STORAGE_BIT : 0) |
                        (flags.persistent ? GL_MAP_PERSISTENT_BIT : 0) |
                        (flags.read ? GL_MAP_READ_BIT : 0) |
                        (flags.write ? GL_MAP_WRITE_BIT : 0);

    bool map = flags.map_after_creation;

    if(!Core::IsWorkerThread()) {
        std::promise<BufferPtr> promise;
        auto futur = promise.get_future();
        BufferPtr buf = BufferPtr(new Buffer());
        if(!Buffer::_Create(buf.get(), mem, flags_i, map))
            promise.set_value(nullptr);
        else
            promise.set_value(buf);
        return futur;
    } else {
        PromiseData<BufferPtr>* pdata = new PromiseData<BufferPtr>();
        auto fut = pdata->promise.get_future();

        Core::Exec([mem, flags_i, map](void* arg) -> uint8_t {
            PromiseData<BufferPtr>* parg = (PromiseData<BufferPtr>*)arg;
            PromiseData<BufferPtr>::Ptr free_guard(parg);

            BufferPtr buf = BufferPtr(new Buffer());
            if(!Buffer::_Create(buf.get(), mem, flags_i, map)) {
                parg->promise.set_value(nullptr);
                return 1;
            }
            parg->promise.set_value(buf);
            return 0;
        }, pdata);

        return fut;
    }
}

std::future<Buffer::MappedMem> Buffer::Map(uint32_t length, Buffer::MapFlags const& flags, uint32_t offset) {
    uint32_t flags_i =  (flags.coherent ? GL_MAP_COHERENT_BIT : 0) |
                        (flags.persistent ? GL_MAP_PERSISTENT_BIT : 0) |
                        (flags.read ? GL_MAP_READ_BIT : 0) |
                        (flags.write ? GL_MAP_WRITE_BIT : 0);

    if(Core::IsWorkerThread()) {
        std::promise<Buffer::MappedMem> promise;
        auto futur = promise.get_future();
        Buffer::_Map(this, offset, length, flags_i);
        promise.set_value(this->_mapState);
        return futur;
    } else {
        PromiseData<Buffer::MappedMem>* pdata = new PromiseData<Buffer::MappedMem>();
        Buffer* buf = this;
        auto fut = pdata->promise.get_future();
        Core::Exec([flags_i, length, offset, buf](void* arg) -> uint8_t{
            PromiseData<Buffer::MappedMem>* parg = (PromiseData<Buffer::MappedMem>*)arg;
            PromiseData<Buffer::MappedMem>::Ptr free_guard(parg);

            Buffer::_Map(buf, offset, length, flags_i);
            parg->promise.set_value(buf->_mapState);

            return 0;
        }, pdata);
        return fut;
    }
}

std::future<void> Buffer::UnMap() {
    if(Core::IsWorkerThread()) {
        std::promise<void> promise;
        auto futur = promise.get_future();
        Buffer::_UnMap(this);
        promise.set_value();
        return futur;
    } else {
        PromiseData<void>* pdata = new PromiseData<void>();
        Buffer* buf = this;
        auto fut = pdata->promise.get_future();

        Core::Exec([buf](void* arg) -> uint8_t{
            PromiseData<void>* parg = (PromiseData<void>*)arg;
            PromiseData<void>::Ptr free_guard(parg);

            Buffer::_UnMap(buf);
            parg->promise.set_value();

            return 0;
        }, pdata);

        return fut;
    }
}

bool Buffer::_Create(Buffer* buf, MemoryPtr const& mem, uint32_t flags, bool map) {
    uint32_t buffer = 0;
    glCreateBuffers(1, &buffer);
    const auto size = mem->GetSize();
    glNamedBufferStorage(buffer, size, mem->GetPtr(), flags);
    buf->_id = buffer;
    buf->_size = size;
    buf->_mapState.mem = nullptr;

    if(map) {
        uint32_t map_flags = (flags & GL_MAP_WRITE_BIT) | (flags & GL_MAP_READ_BIT) | (flags & GL_MAP_PERSISTENT_BIT) | (flags & GL_MAP_COHERENT_BIT);
        return Buffer::_Map(buf, 0, size, map_flags);
    }
    return true;
}

bool Buffer::_Map(Buffer* buf, uint32_t offset, uint32_t length, uint32_t flags) {
    const uint32_t buffer = buf->_id;
    if(buf->IsMapped()) Buffer::_UnMap(buf);
    buf->_mapState.mem = glMapNamedBufferRange(buffer, offset, length, flags);
    buf->_mapState.offset = offset;
    buf->_mapState.length = length;
    return (buf->_mapState.mem != nullptr);
}

bool Buffer::_UnMap(Buffer* buf) {
    glUnmapNamedBuffer(buf->_id);
    buf->_mapState = MappedMem();
    return true;
}

bool Buffer::_MakeResident(Buffer* buf, bool resident) {
}

Buffer::Buffer() : _id(0), _size(0), _mapState() {
}

}
