#pragma once

#include "../build.hpp"
#include "../memory.hpp"

#include <memory>
#include <future>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;

class MR_API Buffer {
    friend class CmdQueue;
public:
    struct MapFlags {
        bool    read = false,
                write = true;
        bool persistent = true;
        bool coherent = true;
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

    static BufferPtr Create(MemoryPtr const& mem, CreationFlags const& flags);
    MappedMem Map(uint32_t length, MapOptFlags const& flags, uint32_t offset = 0); // remaps buffer if needed
    bool UnMap();

    std::future<bool> Write(MemoryPtr const& mem_src, uint32_t offset = 0); // map and write mapped async
    std::future<bool> Read(MemoryPtr const& mem_dst, uint32_t offset = 0); // map and read mapped async

    bool MakeResident(bool read, bool write = false);
    bool MakeNonResident();

    inline MappedMem GetMapState() const; // may be changed during runtime, use mapstate.offset and mapstate.length
    inline uint32_t GetId() const;
    inline bool IsMapped() const;
    inline ResidentState GetResidentState() const;
    inline bool IsResident() const;

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

}
