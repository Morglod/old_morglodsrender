#pragma once

#include "build.hpp"

namespace mr {

class MR_API Core {
public:
    static bool Init();
    static void Shutdown();
    static bool IsCoreThread();
};

}
