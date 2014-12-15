#pragma once

#ifndef _MR_CONTAINERS_H_
#define _MR_CONTAINERS_H_

#include "Thread.hpp"
#include <string.h>
#include <chrono>
#include <queue>
#include <stdlib.h>
#include <initializer_list>

#define MR_CONTAINERS_LOOP_INDEX 0

namespace mr {

template < typename T >
class ITArray {
public:
    virtual T& At(size_t const& i) = 0;
    virtual size_t GetNum() = 0;
};

template < typename T >
class TStaticArray : public ITArray<T> {
public:
    inline T* GetRaw() { return _ar; }
    inline size_t GetNum() override { return _el_num; }
#if MR_CONTAINERS_LOOP_INDEX == 0
    inline T& At(size_t const& i) override { return _ar[i]; }
    inline T* AtPtr(const size_t& i) { return &_ar[i]; }
#else
    inline T& At(size_t i) {
        while(i >= _el_num) i -= _el_num;
        while(i < 0) i += _el_num;
        return _ar[i];
    }
    inline T* AtPtr(size_t i) {
        while(i >= _el_num) i -= _el_num;
        while(i < 0) i += _el_num;
        return &_ar[i];
    }
#endif
    inline void SetDeleteFlag(const bool& d) { _delete = d; }
    inline bool GetDeleteFlag() { return _delete; }

    inline TStaticArray<T> Combine(TStaticArray<T> a) {
        size_t sz = a.GetNum()+GetNum();
        TStaticArray<T> ra = TStaticArray<T>(new T[sz], sz, GetDeleteFlag());
        for(size_t i = 0; i < GetNum(); ++i) ra.At(i) = At(i);
        for(size_t i = 0; i < a.GetNum(); ++i) ra.At(i+GetNum()) = a.At(i);

        return ra;
    }

    TStaticArray<T> Union(TStaticArray<T> a);
    TStaticArray<T> EraseDublicates();

    typedef bool (*ForEachFunc)(TStaticArray<T> const* staticArray, size_t const& index, T* element);
    inline bool ForEach(ForEachFunc func) {
        for(size_t i = 0; i < GetNum(); ++i) {
            if(!func(this, i, AtPtr(i))) return false;
        }
        return true;
    }

    inline TStaticArray<T> RangeRef(size_t const& begin_index, size_t const& end_index) {
        return TStaticArray<T>(&_ar[begin_index], end_index - begin_index, false);
    }

    inline TStaticArray<T> RangeCopy(size_t const& begin_index, size_t const& end_index) {
        T* _ar_cpy = new T[end_index - begin_index];
        for(size_t i = 0; i < (end_index - begin_index); ++i) _ar_cpy[i] = _ar[begin_index+i];
        return TStaticArray<T>(_ar_cpy, end_index - begin_index, true);
    }

    TStaticArray() : _ar(0), _el_num(0), _delete(false) {}
    TStaticArray(const size_t& num) : _ar(new T[num]), _el_num(num), _delete(true) {}
    TStaticArray(T* ar, const size_t& num) : _ar(ar), _el_num(num), _delete(false) {}
    TStaticArray(T* ar, const size_t& num, const bool& del) : _ar(ar), _el_num(num), _delete(del) {}

    TStaticArray(const std::initializer_list<T>& il) : _ar(new T[il.size()]), _el_num(il.size()), _delete(true) {
        for(size_t i = 0; i < il.size(); ++i)
            _ar[i] = *(il.begin() + i);
    }

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

    TStaticArray(TStaticArray<T>& a) : _ar(new T [a._el_num]), _el_num(a._el_num), _delete(a._delete) {
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = a._ar[i];
        }
    }

    inline mr::TStaticArray<T>& operator = (mr::TStaticArray<T> const& ar) {
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

    inline mr::TStaticArray<T>& operator = (mr::TStaticArray<T> & ar) {
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
protected:
    T* _ar = 0;
    size_t _el_num = 0;
    bool _delete = false;
};

template < typename T >
class TDynamicArray : public ITArray<T> {
public:
    class Iterator {
    public:
        inline T& Get() { return _owner->GetRaw()[_el_index]; }
        inline size_t GetIndex() { return _el_index; }
        inline TDynamicArray<T>* GetDArray() { return _owner; }

        inline T& operator -> () { return _owner->GetRaw()[_el_index]; }

        inline Iterator& operator ++() {
            ++_el_index;
            return *this;
        }
        inline Iterator& operator --() {
            --_el_index;
            return *this;
        }

        inline Iterator& operator += (size_t const& offset) {
            _el_index += offset;
            return *this;
        }

        inline Iterator& operator -= (size_t const& offset) {
            _el_index -= offset;
            return *this;
        }

        inline Iterator& operator = (size_t const& i) {
            _el_index = i;
            return *this;
        }

        inline bool operator == (Iterator const& it) {
            if(!_owner || !it._owner) return false;
            return (_el_index == it._el_index) && (_owner->GetRaw() == it._owner->GetRaw());
        }

        inline bool operator != (Iterator const& it) {
            return !(*this == it);
        }

        Iterator() : _el_index(0), _owner(nullptr) {}
        Iterator(TDynamicArray<T>* ar, size_t const& i) : _el_index(i), _owner(ar) {}
        Iterator(const TDynamicArray<T>::Iterator & i) : _el_index(i._el_index), _owner(i._owner) {}
    protected:
        size_t _el_index;
        TDynamicArray<T>* _owner;
    };
    typedef Iterator It;

    inline T* GetRaw() { return _ar; }
    inline size_t GetNum() override { return _el_num; }
    inline size_t GetCapacity() { return _el_cap; }
    inline T& At(size_t const& i) override { return _ar[i]; }
    inline T* AtPtr(size_t const& i) { return &_ar[i]; }

    inline TDynamicArray<T>::Iterator GetFirst() {
        return TDynamicArray<T>::Iterator(this, 0);
    }

    inline TDynamicArray<T>::Iterator GetLast() {
        return TDynamicArray<T>::Iterator(this, _el_num-1);
    }

    inline TDynamicArray<T>::Iterator GetEnd() {
        return TDynamicArray<T>::Iterator(this, _el_num);
    }

    inline void SetDeleteFlag(bool const& b) { _delete = b; }
    inline bool GetDeleteFlag() { return _delete; }

    inline bool Find(T const& t, size_t* index) {
        for(size_t i = 0; i < _el_num; ++i) {
            if(t == _ar[_el_num]) {
                if(index) *index = i;
                return true;
            }
        }
        return false;
    }

    inline void Erase(size_t const& i) {
        _ar[i] = T();
        memmove(&_ar[i], &_ar[i+1], sizeof(T)*(_el_num-i));
        --_el_num;
    }

    inline void Erase(T const& t) {
        size_t index = 0;
        if(Find(t, &index)) {
            Erase(index);
        }
    }

    inline bool PushBack(T const& t) {
        if(_el_cap == 0) if(!SetCapacity(1)) return false;
        if(_el_num >= _el_cap) {
            if(!SetCapacity(_el_num*2)) return false;
        }
        _ar[_el_num] = t;
        ++_el_num;
        return true;
    }

    template<typename... Args>
    inline bool EmplaceBack(Args... args) {
        if(_el_cap == 0) if(!SetCapacity(1)) return false;
        if(_el_num >= _el_cap) {
            if(!SetCapacity(_el_num*2)) return false;
        }
        _ar[_el_num] = T(args...);
        ++_el_num;
        return true;
    }

    inline void PopBack() {
        if(_el_num == 0) return;
        --_el_num;
    }

    inline void Clear() {
        _el_num = 0;
    }

    inline void Free() {
        Clear();
        delete [] _ar;
        _ar = 0;
        _el_cap = 0;
    }

    inline bool SetCapacity(size_t const& num) {
        if(num == _el_cap) return true;
        if(_ar == 0) {
            _ar = new T[num];
        }
        else {
            void * p = realloc(_ar, num*sizeof(T));
            if(!p) {
                _el_cap = 0;
                _el_num = 0;
                return false;
            }
            _ar = (T*)p;
        }
        _el_cap = num;
        if(_el_num > _el_cap) _el_num = _el_cap;
        return true;
    }

    TDynamicArray<T> Union(TStaticArray<T> a);
    TDynamicArray<T> Union(TDynamicArray<T> a);
    TDynamicArray<T> EraseDublicates();
    TDynamicArray<T>& EraseDublicatesInThis();

    inline TDynamicArray<T> Combine(TStaticArray<T> a) {
        size_t sz = a.GetNum()+GetNum();
        TDynamicArray<T> ra = TDynamicArray<T>(sz);
        for(size_t i = 0; i < GetNum(); ++i) ra.PushBack(At(i));
        for(size_t i = 0; i < a.GetNum(); ++i) ra.PushBack(a.At(i));
        return ra;
    }

    inline TDynamicArray<T>& Append(TStaticArray<T> a) {
        for(size_t i = 0; i < a.GetNum(); ++i) PushBack(a.At(i));
        return *this;
    }

    inline TDynamicArray<T> Combine(TDynamicArray<T> a) {
        size_t sz = a.GetNum()+GetNum();
        TDynamicArray<T> ra = TDynamicArray<T>(sz);
        for(size_t i = 0; i < GetNum(); ++i) ra.PushBack(At(i));
        for(size_t i = 0; i < a.GetNum(); ++i) ra.PushBack(a.At(i));
        return ra;
    }

    inline TDynamicArray<T>& Append(TDynamicArray<T> a) {
        for(size_t i = 0; i < a.GetNum(); ++i) PushBack(a.At(i));
        return *this;
    }

    typedef bool (*ForEachFunc)(TDynamicArray<T> const* dynamicArray, size_t const& index, T* element);
    inline bool ForEach(ForEachFunc func) {
        for(size_t i = 0; i < GetNum(); ++i) {
            if(!func(this, i, AtPtr(i))) return false;
        }
        return true;
    }

    inline TStaticArray<T> GetStaticArray() {
        return TStaticArray<T>(_ar, _el_num, false);
    }

    inline TStaticArray<T> ConvertToStaticArray() {
        T* ar = new T[_el_num];
        for(size_t i = 0; i < _el_num; ++i) ar[i] = _ar[i];
        return TStaticArray<T>(ar, _el_num, true);
    }

    TDynamicArray() : _ar(0), _el_num(0), _el_cap(0), _delete(true) {}
    TDynamicArray(size_t const& el_num) : _ar(new T[el_num]), _el_num(el_num), _el_cap(el_num), _delete(true) {}
    TDynamicArray(T* ar, size_t const& el_num) : _ar(ar), _el_num(el_num), _el_cap(el_num), _delete(false) {}
    TDynamicArray(T* ar, size_t const& el_num, bool const& del) : _ar(ar), _el_num(el_num), _el_cap(el_num), _delete(del) {}
    TDynamicArray(TStaticArray<T> ar) : _ar(new T[ar.GetNum()]), _el_num(ar.GetNum()), _el_cap(ar.GetNum()), _delete(true) {}

    TDynamicArray(const std::initializer_list<T>& il) : _ar(0), _el_num(0), _el_cap(0), _delete(true) {
        SetCapacity(il.size());
        _el_num = il.size();
        for(size_t i = 0; i < il.size(); ++i)
            _ar[i] = il[i];
    }

    ~TDynamicArray() {
        if(_delete) Free();
    }

    TDynamicArray(TDynamicArray<T> const& a) : _ar(new T [a._el_num]), _el_num(a._el_num), _el_cap(a._el_cap), _delete(a._delete) {
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = a._ar[i];
        }
    }

    TDynamicArray(TDynamicArray<T>& a) : _ar(new T [a._el_num]), _el_num(a._el_num), _el_cap(a._el_cap), _delete(a._delete) {
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = a._ar[i];
        }
    }

    inline mr::TDynamicArray<T>& operator = (mr::TDynamicArray<T> const& ar) {
        if(this == &ar) {
            return *this;
        }
        _ar = new T [ar._el_num];
        _delete = ar._delete;
        _el_num = ar._el_num;
        _el_cap = ar._el_cap;
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = ar._ar[i];
        }
        return *this;
    }

    inline mr::TDynamicArray<T>& operator = (mr::TDynamicArray<T> & ar) {
        if(this == &ar) {
            return *this;
        }
        _ar = new T [ar._el_num];
        _delete = ar._delete;
        _el_num = ar._el_num;
        _el_cap = ar._el_cap;
        for(size_t i = 0; i < _el_num; ++i){
            _ar[i] = ar._ar[i];
        }
        return *this;
    }
protected:
    T* _ar = 0;
    size_t _el_num = 0, _el_cap = 0;
    bool _delete = false;
};

template< typename T >
TStaticArray<T> TStaticArray<T>::Union(TStaticArray<T> a) {
    TDynamicArray<T> d = a;
    for(size_t i = 0; i < GetNum(); ++i) {
        if(!d.Find(At(i), nullptr)) {
            d.PushBack(At(i));
        }
    }
    return d.ConvertToStaticArray();
}

template< typename T >
TStaticArray<T> TStaticArray<T>::EraseDublicates() {
    TDynamicArray<T> d;
    for(size_t i = 0; i < GetNum(); ++i) {
        if(!d.Find(At(i), nullptr)) {
            d.PushBack(At(i));
        }
    }
    return d.ConvertToStaticArray();
}

template< typename T >
TDynamicArray<T> TDynamicArray<T>::Union(TStaticArray<T> a) {
    TDynamicArray<T> d = a;
    for(size_t i = 0; i < GetNum(); ++i) {
        if(!d.Find(At(i), nullptr)) {
            d.PushBack(At(i));
        }
    }
    return d;
}

template< typename T >
TDynamicArray<T> TDynamicArray<T>::Union(TDynamicArray<T> d) {
    for(size_t i = 0; i < GetNum(); ++i) {
        if(!d.Find(At(i), nullptr)) {
            d.PushBack(At(i));
        }
    }
    return d;
}

template< typename T >
TDynamicArray<T> TDynamicArray<T>::EraseDublicates() {
    TDynamicArray<T> d;
    for(size_t i = 0; i < GetNum(); ++i) {
        if(!d.Find(At(i), nullptr)) {
            d.PushBack(At(i));
        }
    }
    return d;
}

template< typename T >
TDynamicArray<T>& TDynamicArray<T>::EraseDublicatesInThis() {
    for(size_t i = 0; i < GetNum(); ++i) {
        for(size_t i2 = 0; i2 < GetNum(); ++i2) {
            if(i == i2) continue;
            if(At(i) == At(i2)) Erase(i2);
        }
    }
    return *this;
}

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
        memcpy(_queue.GetRaw(), old, sizeof(CacheElement)*old_num); //TODO: 'old' memory leak
        delete [] old;
    }

    void Store(KeyT key, ValueT value) {
        if(_queue.GetNum() == 0) return;

        //find place
        size_t free_place_index = _queue.GetNum()-1;
        for(size_t i = _queue.GetNum()-1; i != 0; --i){
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

    ThreadSafeQueue(const ThreadSafeQueue& c) : _mutex(), _q() {
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
    mu::Mutex _mutex;
    std::queue<elementT> _q;
};

template<typename keyT, class valueT>
class Dictionary {
public:
    inline valueT& operator [] (const keyT& key) {
        for(size_t i = 0; i < _keys.size(); ++i){
            if(_keys[i] == key) return _values[i];
        }
        _keys.push_back(key);
        _values.push_back(valueT());
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

template<typename keyT, class valueT, valueT defaultValue>
class DictionaryDefValue {
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

    DictionaryDefValue() {}
    ~DictionaryDefValue() {}
protected:
    std::vector<keyT> _keys;
    std::vector<valueT> _values;
};

}

#endif // _MR_CONTAINERS_H_
