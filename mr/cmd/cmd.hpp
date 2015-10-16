#pragma once

#include "mr/build.hpp"
#include "mr/buffer/buffer.hpp"

#include <deque>
#include <string.h>

#define MR_CMD_SIZE_CHECK(T) \
static_assert(sizeof(T) <= sizeof(Cmd), "Bad \"" #T "\" type size");

namespace mr {

enum class CmdObjectType : uint8_t {
    None = 0,
    Buffer = 1,
    Texture,

    Num
};

enum class CmdType : uint8_t {
    None = 0,
    BufferMap = 1,

    Num
};

struct Cmd { //32 bytes
    char data[40];

    /** HEADER EXAMPLE
    10 bytes on x64
    CmdObjectType objectType;
    CmdType cmdType;
    void* targetObject;
    **/
};

struct CmdHeader {
    CmdObjectType objectType;
    CmdType cmdType;
    void* targetObject;
};

class MR_API CmdQueue {
public:
    void InsertCmd(Cmd& cmd);

    template<typename T>
    inline void InsertCmd(T& cmd) {
        Cmd c; Cmd_WriteTo(&cmd, &c);
        InsertCmd(c);
    }

    void PopCmd(Cmd* to);
protected:
    static void _ExecUnsafe(Cmd* cmd);
    std::deque<Cmd> _cmds;
};

template<typename T> inline void Cmd_WriteTo (T* from, Cmd* to)  { memcpy(to, from, sizeof(T)); }
template<typename T> inline void Cmd_ReadFrom(Cmd* from, T* to)  { memcpy(to, from, sizeof(T)); }

struct Cmd_BufferMap {
    // HEADER
    CmdObjectType objectType = CmdObjectType::Buffer;
    CmdType cmdType = CmdType::BufferMap;
    Buffer* targetObject;
    // ------

    Buffer::MappedMem* out;
    uint32_t offset,
             length,
             flags;
};

MR_CMD_SIZE_CHECK(Cmd_BufferMap)

}
