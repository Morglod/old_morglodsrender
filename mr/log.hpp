#pragma once

#include "build.hpp"

#include <string>
#include <inttypes.h>

#define MR_LOG(x) mr::Log::Print(x)
#define MR_LOG_D(x) mr::Log::Print( std::string(#x) )
#define MR_LOG_T(x) mr::Log::Print( mr::Log::StrT(x) )
#define MR_LOG_T_STD(x) mr::Log::Print(std::to_string(x))
#define MR_LOG_WARNING(method, reason) mr::Log::Print(mr::Log::Str( std::string(#method) + " " + reason, mr::Log::Type::T_Warning ))
#define MR_LOG_ERROR(method, reason) mr::Log::Print(mr::Log::Str( std::string(#method) + " " + reason, mr::Log::Type::T_Error ))

namespace mr {

class MR_API Log final {
public:
    enum Type : uint8_t {
        T_Info = 0,
        T_Warning = 1,
        T_Error,

        T_Num
    };

    static void Print(std::string const& what);

    static std::string Str(std::string const& msg, mr::Log::Type const& type);

    template<typename T>
    static std::string StrT(T const& t);
};

template<>
inline std::string Log::StrT<std::string>(std::string const& t) {
    return t;
}

template<>
std::string Log::StrT<int>(int const& t);

template<>
std::string Log::StrT<float>(float const& t);

}
