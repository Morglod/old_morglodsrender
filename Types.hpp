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

}

#endif // _MR_TYPES_H_
