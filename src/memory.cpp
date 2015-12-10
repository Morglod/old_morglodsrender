#include "mr/memory.hpp"
#include "src/mp.hpp"

#include <memory>

namespace mr {

MemoryPtr Memory::Ref(void* mem, size_t sz) {
    return MemoryPtr(new Memory(mem, sz, false));
}

MemoryPtr Memory::Copy(const void* mem, size_t sz) {
    MP_BeginSample(Memory::Copy);
    uint8_t* local = new uint8_t[sz]; //malloc(sz);
    //memcpy(local, mem, sz);
    for(size_t i = 0; i < sz; ++i) {
        local[i] = ((const uint8_t*)mem)[i];
    }
    MP_EndSample();
    return MemoryPtr(new Memory(local, sz, true));
}

MemoryPtr Memory::Own(void* mem, size_t sz) {
    return MemoryPtr(new Memory(mem, sz, true));
}

MemoryPtr Memory::Zero(size_t sz) {
    return MemoryPtr(new Memory(nullptr, sz, false));
}

MemoryPtr Memory::New(size_t sz) {
    return MemoryPtr(new Memory(new uint8_t[sz], sz, false));
}

Memory::Memory(void* ptr, size_t sz, bool own) : _ptr(ptr), _size(sz), _own(own) {}
Memory::~Memory() {
    if(_ptr == nullptr) return;
    if(_own) {
        delete [] ((uint8_t*)_ptr);
        _ptr = nullptr;
    }
}

Memory::Memory(Memory const& cpy) : _ptr(cpy._ptr), _size(cpy._size), _own(cpy._own) {
}

}
