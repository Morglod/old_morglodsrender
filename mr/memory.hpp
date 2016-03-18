#pragma once

#include "build.hpp"

#include <memory>
#include <vector>

namespace mr {

typedef std::shared_ptr<class Memory> MemoryPtr;

struct MR_API Memory final {

    static MemoryPtr Ref(void* mem, size_t sz);
    static MemoryPtr Copy(const void* mem, size_t sz);
    static MemoryPtr Own(void* mem, size_t sz);
    static MemoryPtr Zero(size_t sz); // zero memory ptr with size
    static MemoryPtr New(size_t sz);

    static std::vector<MemoryPtr> Ref(std::vector<std::pair<void*, size_t>> const& mem);
    static std::vector<MemoryPtr> Copy(std::vector<std::pair<const void*, size_t>> const& mem);
    static std::vector<MemoryPtr> Own(std::vector<std::pair<void*, size_t>> const& mem);
    static std::vector<MemoryPtr> Zero(std::vector<size_t> const& sz);
    static std::vector<MemoryPtr> New(std::vector<size_t> const& sz);

    inline bool IsOwn() const;
    inline size_t GetSize() const;
    inline void* GetPtr() const;

    template<typename T>
    inline T* As();

    Memory() = delete;
    Memory(void* ptr, size_t sz, bool own);
    Memory(Memory const& cpy);
    Memory& operator = (Memory const&) = delete;
    ~Memory();

private:
    void _Delete();

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

template<typename T>
inline T* Memory::As() {
    if(sizeof(T) > _size) return nullptr;
    return (T*)_ptr;
}

}
