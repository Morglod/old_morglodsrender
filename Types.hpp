#pragma once

#ifndef _MR_TYPES_H_
#define _MR_TYPES_H_

#include <vector>
#include <algorithm>
#include <memory>
#include <string>
#include <string.h>

namespace MR{

class Super {
public:
    virtual std::string ToString();

    Super();
    virtual ~Super();
};

template<typename T>
class Copyable {
public:
    virtual T Copy() = 0;
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

    virtual ~IFlags(){}
};

template<typename flag_t>
class Flags : public Super, public IFlags<flag_t> {
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
    virtual void* TopPtr() = 0;
    virtual size_t TypeSize() = 0; //size of one element in array
    virtual size_t Num() = 0; //num of elements in array
    virtual void* At(const size_t& index) = 0;
};

template<typename _t>
class Array : public IArray {
public:
    inline void* TopPtr() override { return _top; }
    inline size_t TypeSize() override { return sizeof(_t); }
    inline size_t Num() override { return _num; }
    inline void* At(const size_t& index) override { return &_top[index]; }
    inline _t* ElementPtr(const size_t& index) { return &_top[index]; }
    inline _t Element(const size_t& index) { return _top[index]; }

    Array(_t* a, const size_t& num) : _top(a), _num(num) {}
protected:
    _t* _top;
    size_t _num;
};

}

#endif // _MR_TYPES_H_
