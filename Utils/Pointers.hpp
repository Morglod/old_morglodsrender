#pragma once

#ifndef _MR_POINTERS_H_
#define _MR_POINTERS_H_

namespace MR {

template < typename T , typename CounterT = size_t >
class HandlePtr {
public:
    HandlePtr& operator = (T* t) {
        if(_ptr != nullptr) {
            if(*_counter == 1) {
                delete _ptr;
                delete _counter;
            } else --(*_counter);
        }
        _ptr = t;
        _counter = new CounterT(1);
        return *this;
    }

    inline T* Get() { return _ptr; }

    HandlePtr() : _ptr(nullptr), _counter(new CounterT(1)) {}
    HandlePtr(T* t) : _ptr(t), _counter(new CounterT(1)) {}
    HandlePtr(const HandlePtr<T, CounterT>& ptr) : _ptr(ptr._ptr), _counter(ptr._counter) {
        ++(*_counter);
    }
    virtual ~HandlePtr() {
        if(*_counter == 1) {
            delete _ptr;
            delete _counter;
        } else --(*_counter);
    }
protected:
    T* _ptr = nullptr;
    CounterT* _counter;
};

#ifdef _MR_CORE_OBJECTS_H_

#include "../Context.hpp"

template < typename T , typename CounterT = size_t >
class GPUObjectHandlePtr : public HandlePtr<T, CounterT> {
public:
    GPUObjectHandlePtr() : HandlePtr<T, CounterT>() {}
    GPUObjectHandlePtr(T* t) : HandlePtr<T, CounterT>(t) {}
    GPUObjectHandlePtr(const GPUObjectHandlePtr<T, CounterT>& ptr) : HandlePtr<T, CounterT>(ptr) {}

    virtual ~GPUObjectHandlePtr() {
        if(AnyContextAlive()) {
            this->HandlePtr<T, CounterT>::_ptr->Destroy();
        }
    }
};

#endif

}

#endif // _MR_POINTERS_H_
