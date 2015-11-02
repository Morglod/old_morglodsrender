#pragma once

#include "build.hpp"

#include <iostream>

namespace mr {

template<typename T>
class MR_API Json final {
public:
    static bool Export(T* type, std::ostream& out);
    static bool Import(T* type, std::istream& in);
};

}
