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
    inline T* AtPtr(const size_t& i) { return &_ar[i]; }

    StaticArray() : _ar(0), _el_num(0), _delete(false) {}
    StaticArray(T* ar, const size_t& num) : _ar(ar), _el_num(num), _delete(false) {}
    StaticArray(T* ar, const size_t& num, const bool& del) : _ar(ar), _el_num(num), _delete(del) {}
    ~StaticArray() { if(_delete && (_ar)) delete [] _ar; }
protected:
    T* _ar = 0;
    size_t _el_num = 0;
    bool _delete = false;
};

/*
    map-like class with priority sorting and bordered size
*/
template < typename KeyT, typename ValueT >
class PriorityCache {
public:
    class CacheElement {
    public:
        KeyT key;
        ValueT value;
        bool stored = false;
        CacheElement() {}
        CacheElement(KeyT k, ValueT v, const bool& s) : key(k), value(v), stored(s) {}
    };

    void Resize(const size_t& size) {
        //Save old cache
        size_t old_num = _queue.GetNum();
        CacheElement* old = new CacheElement[old_num];
        memcpy(old, _queue.GetRaw(), sizeof(CacheElement)*old_num);

        //Free old cache and create new
        _queue = StaticArray<CacheElement>(new CacheElement[size], size, true);

        //Store old cache
        memcpy(_queue.GetRaw(), old, sizeof(CacheElement)*old_num);
    }

    void Store(KeyT key, ValueT value) {
        if(_queue.GetNum() == 0) return;

        //find place
        size_t free_place_index = _queue.GetNum()-1;
        for(size_t i = _queue.GetNum()-1; i >= 0; --i){
            if( !_queue.At(i).stored ) {
                free_place_index = i;
                break;
            }
        }

        //store
        if(free_place_index > _queue.GetNum()) return;
        *(_queue.AtPtr(free_place_index)) = PriorityCache<KeyT, ValueT>::CacheElement(key, value, true);
    }

    void Swap(const size_t& a, const size_t& b) {
        KeyT _k = _queue.At(a).key;
        ValueT _v = _queue.At(a).value;
        bool _s = _queue.At(a).stored;

        *(_queue.AtPtr(a)) = _queue.At(b);
        *(_queue.AtPtr(b)) = PriorityCache<KeyT, ValueT>::CacheElement(_k, _v, _s);
    }

    void RisePriority(const size_t& index) {
        if(index == 0) return;
        if(index >= _queue.GetNum()) return;
        Swap(index, index-1);
    }

    bool Get(KeyT key, ValueT & value) {
        for(size_t i = 0; i < _queue.GetNum(); ++i){
            if(_queue.At(i).key == key) {
                value = _queue.At(i).value;
                RisePriority(i);
                return true;
            }
        }
        return false;
    }

    StaticArray<PriorityCache<KeyT, ValueT>::CacheElement> * GetQueuePtr() { return &_queue; }

    PriorityCache() : _queue() {}
    PriorityCache(const size_t& size) : _queue(new PriorityCache<KeyT, ValueT>::CacheElement[size], size, true) {}
    ~PriorityCache() {}
protected:
    StaticArray<PriorityCache<KeyT, ValueT>::CacheElement> _queue;
};

}

#endif // _MR_CONTAINERS_H_
