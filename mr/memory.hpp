#pragma once

#include "build.hpp"

#include <memory>

namespace mr {

typedef std::shared_ptr<class Memory> MemoryPtr;

struct MR_API Memory final {
    inline bool IsOwn() const;
    inline size_t GetSize() const;
    inline void* GetPtr() const;

    Memory() = delete;
    Memory(void* ptr, size_t sz, bool own);
    Memory(Memory const& cpy);
    Memory& operator = (Memory const&) = delete;
    ~Memory();

    static MemoryPtr Ref(void* mem, size_t sz);
    static MemoryPtr Copy(void* mem, size_t sz);
    static MemoryPtr Own(void* mem, size_t sz);
    static MemoryPtr Zero();
private:
    void* _ptr = nullptr;
    size_t _size = 0;
    bool _own = true; // if true, delete at Memory object deletion.
};

inline bool Memory::IsOwn() const {
    return _own;
}

inline size_t Memory::GetSize() const {
    return _size;
}

inline void* Memory::GetPtr() const {
    return _ptr;
}

}
