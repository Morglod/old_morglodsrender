#pragma once

#include "build.hpp"
#include "memory.hpp"

#include <memory>
#include <future>

#define MR_RESIDENT_READ_ONLY true, false
#define MR_RESIDENT_WRITE_ONLY false, true
#define MR_RESIDENT_READ_WRITE true, true

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;

struct MR_API BufferInfo {
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

    void Get(uint32_t buffer);
};

class MR_API Buffer {
    friend class CmdQueue;
    friend class Draw;
    friend class VertexBuffer;
    friend class IndexBuffer;
public:
    struct MapFlags {
        bool    read = false,
                write = false;
        bool persistent = false;
        bool coherent = false;
        MapFlags() = default;
        MapFlags(MapFlags const&);
    };

    struct MapOptFlags : MapFlags {
        bool invalidate_range = false;
        bool invalidate_buffer = false;
        bool flush_explicit = false;
        bool unsynchronized = false;
        MapOptFlags() = default;
        MapOptFlags(MapFlags const&);
    };

    struct CreationFlags : public MapFlags {
        bool dynamic = false;
        bool client_storage = false;
        bool map_after_creation = false;
        CreationFlags() = default;
    };

    struct MappedMem {
        void* mem = nullptr;
        uint32_t offset = 0;
        uint32_t length = 0;
        uint32_t flags = 0;
    };

    struct ResidentState {
        bool resident = false;
        bool read = true;
        bool write = false;
        uint64_t address = 0;
    };

    // Memory may be Memory::Zero
    static BufferPtr Create(MemoryPtr const& mem, CreationFlags const& flags);
    void Destroy();

    // Read OpenGL docs, about flags
    MappedMem Map(uint32_t length, MapOptFlags const& flags, uint32_t offset = 0); // remaps buffer if needed
    bool UnMap();
    void FlushMapped();

    // Map memory range in [offset, offset + mem.size] (remap if needed) and write/read async
    std::future<bool> WriteAsync(MemoryPtr const& mem_src, uint32_t offset = 0); // map and write mapped async
    std::future<bool> ReadAsync(MemoryPtr const& mem_dst, uint32_t offset = 0); // map and read mapped async

    bool Write(MemoryPtr const& mem_src, uint32_t offset = 0);
    bool Read(MemoryPtr const& mem_dst, uint32_t offset = 0);

    // Update ResidentState
    bool MakeResident(bool read, bool write = false);
    bool MakeNonResident();

    inline MappedMem GetMapState() const; // may be changed during runtime, use mapstate.offset and mapstate.length
    inline uint32_t GetId() const;
    inline bool IsMapped() const;
    inline ResidentState GetResidentState() const;
    inline uint64_t GetResidentHandle() const;
    inline bool IsResident() const;
    inline int32_t GetSize() const;

    virtual ~Buffer();

protected:
    Buffer();

    uint32_t _id;
    int32_t _size;
    MappedMem _mapState;
    ResidentState _resident;
};

inline Buffer::MappedMem Buffer::GetMapState() const {
    return _mapState;
}

inline uint32_t Buffer::GetId() const {
    return _id;
}

inline bool Buffer::IsMapped() const {
    return _mapState.mem != nullptr;
}

inline Buffer::ResidentState Buffer::GetResidentState() const {
    return _resident;
}

inline bool Buffer::IsResident() const {
    return _resident.resident;
}

inline int32_t Buffer::GetSize() const {
    return _size;
}

inline uint64_t Buffer::GetResidentHandle() const {
    return _resident.address;
}

}
