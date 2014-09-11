#pragma once

#ifndef _MR_CONTAINERS_H_
#define _MR_CONTAINERS_H_

#include "Threads.hpp"
#include <string.h>
#include <chrono>
#include <queue>

namespace MR {

template < typename T >
class TStaticArray {
public:
    inline T* GetRaw() { return _ar; }
    inline size_t GetNum() { return _el_num; }
    inline T& At(const size_t& i) { return _ar[i]; }
    inline T* AtPtr(const size_t& i) { return &_ar[i]; }
    inline void DeleteFlag(const bool& d) { _delete = d; }
    inline bool GetDeleteFlag() { return _delete; }

    inline TStaticArray<T> Combine(TStaticArray<T> a) {
        size_t sz = a.GetNum()+GetNum();
        TStaticArray<T> ra = TStaticArray<T>(new T[sz], sz, GetDeleteFlag());
        size_t sz1 = sizeof(T) * GetNum();
        memcpy(ra.GetRaw(), GetRaw(), sz1);
        memcpy((void*)((size_t)ra.GetRaw() + sz1), a.GetRaw(), sizeof(T) * a.GetNum());
        return ra;
    }

    TStaticArray() : _ar(0), _el_num(0), _delete(false) {}
    TStaticArray(T* ar, const size_t& num) : _ar(ar), _el_num(num), _delete(false) {}
    TStaticArray(T* ar, const size_t& num, const bool& del) : _ar(ar), _el_num(num), _delete(del) {}
    ~TStaticArray() {
        if(_delete && _ar) {
            delete [] _ar;
            _ar = 0;
        }
    }

    TStaticArray(TStaticArray<T> const& a) : _ar(new T [a._el_num]), _el_num(a._el_num), _delete(a._delete) {
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = a._ar[i];
        }
    }

    inline MR::TStaticArray<T>& operator = (MR::TStaticArray<T> const& ar) {
        if(this == &ar) {
            return *this;
        }
        _ar = new T [ar._el_num];
        _delete = ar._delete;
        _el_num = ar._el_num;
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = ar._ar[i];
        }
        return *this;
    }

    inline MR::TStaticArray<T>& operator = (MR::TStaticArray<T> & ar) {
        if(this == &ar) {
            return *this;
        }
        _ar = ar._ar;
        _delete = ar._delete;
        _el_num = ar._el_num;
        ar._delete = false;
        return *this;
    }
protected:
    T* _ar = 0;
    size_t _el_num = 0;
    bool _delete = false;
};
/*
template < typename T >
class TDynamicArrayTimed {
    inline T* GetRaw() { return _ar; }
    inline size_t GetNum() { return _el_num; }
    inline T& At(const size_t& i) { return _ar[i]; }
    inline T* AtPtr(const size_t& i) { return &_ar[i]; }
    inline void DeleteFlag(const bool& d) { _delete = d; }
    inline bool GetDeleteFlag() { return _delete; }

    inline void PushBack(T t) {
    }

    inline bool Resize(size_t const& num) {
        void * p = realloc(_ar, num*sizeof(T));
        if(!p) return false;
        _el_capacity = num;
        if(_el_num < _el_capacity) _el_num = _el_capacity;
        _ar = p;
        return true;
    }
protected:
    T* _ar = 0;
    size_t _el_num = 0;
    size_t _el_capacity = 0;
    bool _delete = false;
    std::chrono::milliseconds _last_mem_reallocated;
};
*/

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
        _queue = TStaticArray<CacheElement>(new CacheElement[size], size, true);

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

    TStaticArray<PriorityCache<KeyT, ValueT>::CacheElement> * GetQueuePtr() { return &_queue; }

    PriorityCache() : _queue() {}
    PriorityCache(const size_t& size) : _queue(new PriorityCache<KeyT, ValueT>::CacheElement[size], size, true) {}
    ~PriorityCache() {}
protected:
    TStaticArray<PriorityCache<KeyT, ValueT>::CacheElement> _queue;
};

template< typename elementT >
class ThreadSafeQueue {
public:
    void Push(const elementT& e) {
        _mutex.Lock();
        _q.push(e);
        _mutex.UnLock();
    }

    bool Pop(elementT& e) {
        _mutex.Lock();
        if(_q.empty()) return false;
        e = _q.front();
        _q.pop();
        _mutex.UnLock();
        return true;
    }

    bool GetFront(elementT& e) {
        _mutex.Lock();
        if(_q.empty()) return false;
        e = _q.front();
        _q.pop();
        _mutex.UnLock();
        return true;
    }

    bool IsEmpty() {
        _mutex.Lock();
        bool b = _q.empty();
        _mutex.UnLock();
        return b;
    }

    size_t GetSize() {
        _mutex.Lock();
        size_t n = _q.size();
        _mutex.UnLock();
        return n;
    }

    ThreadSafeQueue() : _mutex(), _q() {}

    ThreadSafeQueue(const ThreadSafeQueue& c) {
        c._mutex.Lock();
        _q = c._q;
        c._mutex.UnLock();
    }

    virtual ~ThreadSafeQueue() {
        _mutex.Lock();
        while(!_q.empty()) { _q.pop(); }
        _mutex.UnLock();
    }
protected:
    MR::Mutex _mutex;
    std::queue<elementT> _q;
};

template<typename keyT, typename valueT, valueT defaultValue>
class Dictionary {
public:
    const valueT DefaultValue = defaultValue;

    inline valueT& operator [] (const keyT& key) {
        for(size_t i = 0; i < _keys.size(); ++i){
            if(_keys[i] == key) return _values[i];
        }
        _keys.push_back(key);
        _values.push_back(defaultValue);
        return _values[_values.size()-1];
    }

    inline size_t GetNum() {
        return _keys.size();
    }

    inline bool ContainKey(const keyT& key) {
        for(size_t i = 0; i < _keys.size(); ++i){
            if(_keys[i] == key) return true;
        }
        return false;
    }

    inline void Remove(const keyT& key) {
        for(size_t i = 0; i < _keys.size(); ++i){
            if(_keys[i] == key) {
                _keys.erase(_keys.begin()+i);
                _values.erase(_values.begin()+i);
            }
        }
    }

    inline void Add(const keyT& key, const valueT& value) {
        _keys.push_back(key);
        _values.push_back(value);
    }

    inline void Clear() {
        _keys.clear();
        _values.clear();
    }

    inline keyT* GetKeysPtr() {
        return &_keys[0];
    }

    inline valueT* GetValuesPtr() {
        return &_values[0];
    }

    inline keyT& KeyAt(const size_t& i) {
        return _keys[i];
    }

    inline valueT& ValueAt(const size_t& i) {
        return _values[i];
    }

    Dictionary() {}
    ~Dictionary() {}
protected:
    std::vector<keyT> _keys;
    std::vector<valueT> _values;
};

}

#endif // _MR_CONTAINERS_H_
