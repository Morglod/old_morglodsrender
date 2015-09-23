#pragma once

#include "../build.hpp"
#include "../memory.hpp"

#include <memory>
#include <future>

namespace mr {

typedef std::shared_ptr<class Buffer> BufferPtr;

class MR_API Buffer {
public:
    struct MapCmd {
        bool    read = false,
                write = true;
        bool persistent = true;
        bool coherent = true;
        MapCmd() = default;
    };

    struct CreationCmd : public MapCmd {
        bool dynamic = false;
        bool client_storage = false;
        bool map_after_creation = false;
        CreationCmd() = default;
    };

    struct MappedMem {
        void* mem = nullptr;
        uint32_t offset = 0;
        uint32_t length = 0;
    };

    static std::future<BufferPtr> Create(MemoryPtr const& mem, CreationCmd const& cmd);
    std::future<MappedMem> Map(uint32_t length, MapCmd const& cmd, uint32_t offset = 0);
    std::future<void> UnMap();

    //static std::future<bool> Write(MemoryPtr const& mem); map and write mapped async
    inline MappedMem GetMappedMem() const { return _mapped; }
    inline uint32_t GetId() const { return _id; }
protected:
    Buffer();
private:
    static bool _Create(Buffer* buf, MemoryPtr const& mem, uint32_t flags, bool map);
    static bool _Map(Buffer* buf, uint32_t offset, uint32_t length, uint32_t flags);
    static bool _UnMap(Buffer* buf);

    uint32_t _id;
    int32_t _size;
    MappedMem _mapped;
};

}
