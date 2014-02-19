#pragma once

#ifndef _MR_TYPES_H_
#define _MR_TYPES_H_

#include <vector>
#include <memory>

namespace MR{

template<typename T>
class Copyable {
public:
    virtual T Copy() = 0;
};

template<typename ThisType>
class WithPtr {
public:
    typedef std::shared_ptr<ThisType> Ptr;
};

template<typename flag_t>
class Flags{
public:
    inline void Add(const flag_t& f){_flags.push_back(f);}
    inline void Remove(const flag_t& f){typename std::vector<flag_t>::iterator it = std::find(_flags.begin(), _flags.end(), f); if(it==_flags.end())return; _flags.erase(it);}
    inline bool Contains(const flag_t& f){auto it = std::find(_flags.begin(), _flags.end(), f); return (it!=_flags.end());}
    inline flag_t* Data(){return _flags.data();}
    inline typename std::vector<flag_t>::size_type Size(){return _flags.size();}

    inline void operator += (const flag_t& t){Add(t);}
    inline void operator -= (const flag_t& t){Remove(t);}

    Flags() : _flags() {}
protected:
    std::vector<flag_t> _flags;
};

}

#endif // _MR_TYPES_H_
