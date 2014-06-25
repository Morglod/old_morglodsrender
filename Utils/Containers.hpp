#pragma once

#ifndef _MR_CONTAINERS_H_
#define _MR_CONTAINERS_H_

#include <stddef.h>

namespace MR {

template < typename T >
class StaticArray {
public:
    inline T* GetRaw() { return _ar; }
    inline size_t GetNum() { return _el_num; }
    inline T& At(const size_t& i) { return _ar[i]; }

    StaticArray() : _ar(0), _el_num(0), _delete(false) {}
    StaticArray(T* ar, const size_t& num) : _ar(ar), _el_num(num), _delete(false) {}
    StaticArray(T* ar, const size_t& num, const bool& del) : _ar(ar), _el_num(num), _delete(del) {}
    ~StaticArray() { if(_delete && (_ar)) delete [] _ar; }
protected:
    T* _ar;
    size_t _el_num;
    bool _delete;
};

}

#endif // _MR_CONTAINERS_H_
