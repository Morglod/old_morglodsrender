#pragma once

#ifndef _MR_TYPES_H_
#define _MR_TYPES_H_

#include <vector>
#include <algorithm>
#include <memory>
#include <string>

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

class IArray{
public:
    virtual void* FirstElement() = 0;
    virtual size_t Size() = 0; //num of elements
    virtual void* At(const size_t& index) = 0;
};

template<typename t>
class Array : public IArray {
public:
    void* FirstElement() override { return _first_element_ptr; }
    size_t Size() override { return _size; }
    void* At(const size_t& index) override { return (void*)&_first_element_ptr[index]; }
    t operator[](const size_t& index) {return _first_element_ptr[index];}
    Array(t* a, size_t sz) : _first_element_ptr(a), _size(sz) {}
protected:
    t* _first_element_ptr;
    size_t _size;
};

}

#endif // _MR_TYPES_H_
