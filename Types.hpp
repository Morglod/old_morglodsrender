#pragma once

#ifndef _MR_TYPES_H_
#define _MR_TYPES_H_

#include <vector>
#include <algorithm>
#include <memory>
#include <string>
#include <string.h>
#include <bitset>

#include <glm/glm.hpp>

namespace MR {

class Object {
public:
    virtual std::string ToString();

    Object();
    virtual ~Object();
};

template<typename T>
class Copyable {
public:
    virtual T Copy() = 0;
};

template<typename T>
class Comparable {
public:
    virtual bool Equal(T) = 0;
};

template<typename flag_t>
class IFlags {
public:
    virtual void Add(const flag_t& f) = 0;
    virtual void Remove(const flag_t& f) = 0;
    virtual bool Contains(const flag_t& f) = 0;
    virtual flag_t* Data() = 0;
    virtual typename std::vector<flag_t>::size_type Size() = 0;

    virtual void operator += (const flag_t& t) = 0;
    virtual void operator -= (const flag_t& t) = 0;
};

template<typename flag_t>
class Flags : public Object, public IFlags<flag_t> {
public:
    void Add(const flag_t& f) override;
    void Remove(const flag_t& f) override;
    bool Contains(const flag_t& f) override;
    flag_t* Data() override;
    typename std::vector<flag_t>::size_type Size() override;

    void operator += (const flag_t& t) override;
    void operator -= (const flag_t& t) override;

    std::string ToString() override;

    Flags();
    virtual ~Flags();
protected:
    std::vector<flag_t> _flags;
};

class IArray {
public:
    virtual void* TopPtr() = 0; //first element
    virtual size_t TypeSize() = 0; //size of one element in array
    virtual size_t Num() = 0; //num of elements in array
    virtual void* At(const size_t& index) = 0;
};

template<typename _t>
class Array : public IArray, public Copyable< MR::Array<_t> >, public Comparable<IArray*> {
public:
    inline void* TopPtr() override {
        return _top;
    }
    inline size_t TypeSize() override {
        return sizeof(_t);
    }
    inline size_t Num() override {
        return _num;
    }
    inline void* At(const size_t& index) override {
        return &_top[index];
    }

    Array Copy() override {
        _t* a = new _t[_num];
        for(size_t i = 0; i < _num; ++i) {
            a[i] = _top[i];
        }
        return Array(a, _num);
    }

    bool Equal(IArray * a) override {
        if(a->Num() != _num) return false;
        if(a->TypeSize() != sizeof(_t)) return false;
        for(size_t i = 0; i < _num; ++i) {
            if( !( ((_t*)(a->TopPtr()))[i] == _top[i]) ) return false;
        }
        return true;
    }

    inline _t* GetPtr(const size_t& index) {
        return &_top[index];
    }
    inline _t Get(const size_t& index) {
        return _top[index];
    }
    inline void Set(const size_t& index, _t value) {
        _top[index] = value;
    }
    inline size_t TotalSize() {
        return sizeof(_t) * _num;
    }

    Array(const size_t& num) : _top( new _t[num] ), _num(num) {}
    Array(_t* a, const size_t& num) : _top(a), _num(num) {}
    virtual ~Array() {
        delete [] _top;
    }
protected:
    _t* _top;
    size_t _num;
};

template<typename ParentT, typename ChildrenT>
class NodeArray {
public:
    ParentT* parent;
    std::vector<ChildrenT*> children;
};

union Byte {
    unsigned char byte;

    struct {
        bool bit1 : 1;
        bool bit2 : 1;
        bool bit3 : 1;
        bool bit4 : 1;
        bool bit5 : 1;
        bool bit6 : 1;
        bool bit7 : 1;
        bool bit8 : 1;
    };

    std::string ToString() {
        return std::to_string(bit8) + std::to_string(bit7) + std::to_string(bit6) + std::to_string(bit5) + std::to_string(bit4) + std::to_string(bit3) + std::to_string(bit2) + std::to_string(bit1);
    }
};

inline void GetBits(const unsigned char& byte, bool* bits){
    Byte b;
    b.byte = byte;
    bits[0] = b.bit1;
    bits[1] = b.bit2;
    bits[2] = b.bit3;
    bits[3] = b.bit4;
    bits[4] = b.bit5;
    bits[5] = b.bit6;
    bits[6] = b.bit7;
    bits[7] = b.bit8;
}

inline void SetBits(unsigned char * byte, bool bits[8]){
    *byte = (1 * bits[0]) +
            (2 * bits[1]) +
            (4 * bits[2]) +
            (8 * bits[3]) +
            (16 * bits[4]) +
            (32 * bits[5]) +
            (64 * bits[6]) +
            (128 * bits[7]);
}

class ICollidable {
public:
    virtual bool TestPoint(const glm::vec3& point) = 0;
    virtual bool TestAABB(const glm::vec3& min, const glm::vec3& size) = 0;
    virtual bool TestSphere(const glm::vec3& point, const float& radius) = 0;
};

}

#endif // _MR_TYPES_H_
