#include "mr/memory.hpp"
#include "src/mp.hpp"
#include "mr/alloc.hpp"

#include <memory>

namespace mr {

MemoryPtr Memory::Ref(void* mem, size_t sz) {
    MP_ScopeSample(Memory::Ref);
    return MR_NEW_SHARED(Memory, mem, sz, false);
}

std::vector<MemoryPtr> Memory::Ref(std::vector<std::pair<void*, size_t>> const& mem) {
    MP_ScopeSample(Memory::Ref);
    std::vector<MemoryPtr> result(mem.size());
    for(size_t i = 0; i < mem.size(); ++i) {
        result[i] = Memory::Ref(mem[i].first, mem[i].second);
    }
    return result;
}

MemoryPtr Memory::Copy(const void* mem, size_t sz) {
    MP_ScopeSample(Memory::Copy);
    uint8_t* local = MR_NEW_ARRAY(uint8_t, sz);
    //memcpy(local, mem, sz);
    for(size_t i = 0; i < sz; ++i) {
        local[i] = ((const uint8_t*)mem)[i];
    }
    return MR_NEW_SHARED(Memory, local, sz, true);
}

std::vector<MemoryPtr> Memory::Copy(std::vector<std::pair<const void*, size_t>> const& mem) {
    MP_ScopeSample(Memory::Copy);
    std::vector<MemoryPtr> result(mem.size());
    for(size_t i = 0; i < mem.size(); ++i) {
        result[i] = Memory::Copy(mem[i].first, mem[i].second);
    }
    return result;
}

MemoryPtr Memory::Own(void* mem, size_t sz) {
    MP_ScopeSample(Memory::Own);
    return MR_NEW_SHARED(Memory, mem, sz, true);
}

std::vector<MemoryPtr> Memory::Own(std::vector<std::pair<void*, size_t>> const& mem) {
    MP_ScopeSample(Memory::Own);
    std::vector<MemoryPtr> result(mem.size());
    for(size_t i = 0; i < mem.size(); ++i) {
        result[i] = Memory::Own(mem[i].first, mem[i].second);
    }
    return result;
}

MemoryPtr Memory::Zero(size_t sz) {
    MP_ScopeSample(Memory::Zero);
    return MR_NEW_SHARED(Memory, nullptr, sz, false);
}

std::vector<MemoryPtr> Memory::Zero(std::vector<size_t> const& sz) {
    MP_ScopeSample(Memory::Zero);
    std::vector<MemoryPtr> result(sz.size());
    for(size_t i = 0; i < sz.size(); ++i) {
        result[i] = Memory::Zero(sz[i]);
    }
    return result;
}

MemoryPtr Memory::New(size_t sz) {
    MP_ScopeSample(Memory::New);
    return MR_NEW_SHARED(Memory, MR_NEW_ARRAY(uint8_t, sz), sz, true);
}

std::vector<MemoryPtr> Memory::New(std::vector<size_t> const& sz) {
    MP_ScopeSample(Memory::New);
    std::vector<MemoryPtr> result(sz.size());
    for(size_t i = 0; i < sz.size(); ++i) {
        result[i] = Memory::New(sz[i]);
    }
    return result;
}

void Memory::_Delete() {
    MP_ScopeSample(Memory::_Delete);
    if(_ptr == nullptr) return;
    if(_own) {
        delete [] ((uint8_t*)_ptr);
        _ptr = nullptr;
    }
}

Memory::Memory(void* ptr, size_t sz, bool own) : _ptr(ptr), _size(sz), _own(own) {
}

Memory::~Memory() {
    _Delete();
}

Memory::Memory(Memory const& cpy) : _ptr(cpy._ptr), _size(cpy._size), _own(cpy._own) {
}

}
