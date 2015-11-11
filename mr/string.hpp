#pragma once

#include <string>

namespace mr {

inline void ReplaceString(std::string& where, std::string const& what, std::string const& to) {
    size_t it = -1;
    while((it = where.find(what)) != -1) {
        where = where.replace(it, what.size(), to);
    }
}

}
