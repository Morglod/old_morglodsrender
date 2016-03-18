#include "mr/alloc.hpp"

#include <iostream>
#include <memory>
/*
struct Pool {
    int8_t* heap = nullptr;

    Pool(int8_t* ptr) : heap(ptr) {
        std::cout << "POOL ALLOC";
    }
    ~Pool() {
        //if(heap) free(heap);
        //std::cout << "POOL FREE";
    }
};

std::shared_ptr<Pool> _pool = nullptr;

namespace mr {

void* _Alloc::Alloc(const size_t sz) {
    if(_pool == nullptr) {
        _pool = std::shared_ptr<Pool>(new Pool((int8_t*)malloc(314572800)));
    }
    static size_t heap_offset = 0;
    void* ptr = &_pool->heap[heap_offset];
    heap_offset += sz;
    return ptr;
}

}
*/
