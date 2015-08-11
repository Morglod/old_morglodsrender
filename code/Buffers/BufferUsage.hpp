#pragma once

namespace mr {

enum class BufferUsage : int8_t {
    Static = 0,
    FastChange = 1,
    FastReadWrite = 2
};

}
