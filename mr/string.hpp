#pragma once

#include <string>

namespace mr {

inline std::string ReplaceString(std::string where, std::string const& what, std::string const& to) {
    size_t it = -1;
    while((it = where.find(what)) != std::string::npos) {
        where = where.replace(it, what.size(), to);
    }
    return where;
}

}
