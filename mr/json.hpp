#pragma once

#include "build.hpp"

#include <iostream>


#define _MR_JSON_MEMBER(jsonValue, memberName, memberTypeCheckCall, callingMethod, outputVar, outputTypeCall) \
if(! jsonValue .isMember( memberName )) { \
    MR_LOG_ERROR( callingMethod , "\"" memberName "\" is not a member" ); \
    return false; \
} else { \
    ::Json::Value json_value = jsonValue .get( memberName , ::Json::Value::null); \
    if(! json_value . memberTypeCheckCall ()) { \
        MR_LOG_ERROR( callingMethod, "\"" memberName "\" member is not right type"); \
        return false; \
    } \
    outputVar = json_value . outputTypeCall (); \
}

#define _MR_JSON_HEADER( _T_ ) \
namespace mr { \
template<> bool mr::Json<_T_>::Export(_T_& type, std::ostream& out); \
template<> bool mr::Json<_T_>::Import(_T_& type, std::istream& in); \
template<> bool mr::Json<_T_>::_Export(_T_& type, void* out); \
template<> bool mr::Json<_T_>::_Import(_T_& type, void* in); \
} \
template<>  \
MR_API bool mr::Json<_T_>::Export(_T_& type, std::ostream& out) { \
    MP_ScopeSample(mr::Json<_T_>::Export); \
    ::Json::Value j_root; \
    if(!_Export(type, &j_root)) return false; \
    out << j_root; \
    return true; \
} \
template<> \
MR_API bool mr::Json<_T_>::Import(_T_& type, std::istream& in) { \
    MP_ScopeSample(mr::Json<_T_>::Import); \
    ::Json::Value j_root; \
    in >> j_root; \
    return _Import(type, &j_root); \
}

namespace mr {

template<typename T>
class MR_API Json final {
public:
    static bool Export(T& type, std::ostream& out);
    static bool Import(T& type, std::istream& in);

    static bool _Export(T& type, void* json_value_out);
    static bool _Import(T& type, void* json_value_in);
};

}
