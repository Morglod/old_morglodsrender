#include "Types.hpp"
#include <iostream>

namespace MR {

template<typename flag_t>
void Flags<flag_t>::Add(const flag_t& f){
    _flags.push_back(f);
}

template<typename flag_t>
void Flags<flag_t>::Remove(const flag_t& f){
    typename std::vector<flag_t>::iterator it = std::find(_flags.begin(), _flags.end(), f); if(it==_flags.end())return; _flags.erase(it);
}

template<typename flag_t>
bool Flags<flag_t>::Contains(const flag_t& f){
    auto it = std::find(_flags.begin(), _flags.end(), f); return (it!=_flags.end());
}

template<typename flag_t>
flag_t* Flags<flag_t>::Data(){
    return _flags.data();
}

template<typename flag_t>
typename std::vector<flag_t>::size_type Flags<flag_t>::Size(){
    return _flags.size();
}

template<typename flag_t>
void Flags<flag_t>::operator += (const flag_t& t){
    Add(t);
}

template<typename flag_t>
void Flags<flag_t>::operator -= (const flag_t& t){
    Remove(t);
}

template<typename flag_t>
std::string Flags<flag_t>::ToString(){
    return "Flags : IFlags class";
}

template<typename flag_t>
Flags<flag_t>::Flags() : IObject(), _flags() {
}

template<typename flag_t>
Flags<flag_t>::~Flags(){
}

}
