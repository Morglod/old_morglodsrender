#pragma once

/**

TEST ONLY

**/

namespace mr {

struct Cmd_CreateBuffer {
    MemoryPtr mem;
    Buffer::CreationFlags flags;
    BufferPtr result = nullptr;
};

struct Cmd_CreateShader {
    ShaderType type;
    std::string code;
    ShaderPtr result = nullptr;
};

struct Cmd_CreateProgram {
    std::vector<ShaderPtr> shaders;
    ShaderProgramPtr result = nullptr;
};

struct Cmd_DrawClear {
    uint32_t flags;
};

struct Cmd_DrawPrimitive {
    ShaderProgramPtr program;
    DrawMode drawMode;
    VertexBufferPtr vbuffer;
    IndexBufferPtr ibuffer;
};

struct Cmd_DrawSwap {
    uint8_t padding;
};

struct Cmd_DrawClearColor {
    uint8_t r, g, b, a;
};

}
