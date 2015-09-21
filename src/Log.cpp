#include "../mr/Log.hpp"
#include "../mr/Build_Log.hpp"

#include <string>

#if MR_LOG_USE_IOSTREAM == 1
    #include <iostream>
#endif

#if MR_LOG_USE_STD == 1
    #include <stdio.h>
#endif

#if MR_LOG_USE_FILE == 1
    #include <fstream>

namespace {
std::ofstream _log_file(MR_LOG_FILE);
}
#endif

namespace mr {

void Log::Print(std::string const& what) {
#if MR_LOG_USE_IOSTREAM == 1
    std::cout.write(what.data(), what.size());
    std::endl(std::cout);
#endif

#if MR_LOG_USE_STD == 1
    sprintf(stdout, what.data());
    sprintf(stdout, "\n");
#endif

#if MR_LOG_USE_FILE == 1
    _log_file.write(what.data(), what.size());
    std::endl(_log_file);
#endif
}

std::string Log::Str(std::string const& msg, mr::Log::Type const& type) {
    if(type == 0) return msg;
    std::string pre[Log::Type::T_Num] = {
        "",
        "Warning. ",
        "Error! "
    };
    return pre[type]+msg;
}

template<>
std::string Log::StrT<int>(int const& t) {
    return std::to_string(t);
}

template<>
std::string Log::StrT<float>(float const& t) {
    return std::to_string(t);
}

}
