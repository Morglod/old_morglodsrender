#pragma once

#ifndef _MR_SHADER_COMPILER_H_
#define _MR_SHADER_COMPILER_H_

#include "../Utils/Containers.hpp"

#include <string>

/**
    Class used to compile shader's code, optimization and errors processing.
*/

namespace MR {

struct ShaderCompilationMessage {
public:
    enum MessageType { MT_Info, MT_Warning, MT_Error };

    inline MessageType GetType() { return _type; }
    inline int GetCode() { return _code; }
    inline const std::string& GetText() { return _text; }
    inline const unsigned int& GetLine() { return _line; }

    ShaderCompilationMessage() {}
    ShaderCompilationMessage(const MessageType& type,
                             const int& code,
                             const unsigned int& line,
                             const std::string& text);

protected:
    MessageType _type = MT_Info;
    int _code = 0;
    unsigned int _line = 0;
    std::string _text = "";
};

class ShaderCompilationOutput {
public:
    inline StaticArray<ShaderCompilationMessage>& GetMessages() { return _messages; }
    inline ShaderCompilationMessage& GetMessage(const size_t& i) { return _messages.At(i); }
    inline size_t GetMessagesNum() { return _messages.GetNum(); }
    inline bool Good() { return _result; }

    /// By default result is false
    ShaderCompilationOutput();

    /// Memory, allocated for array, will be free on ShaderCompilationOutput deletion
    ShaderCompilationOutput(ShaderCompilationMessage* ar, const size_t& num, const bool& result);

    ShaderCompilationOutput(const StaticArray<ShaderCompilationMessage>& msg, const bool& result);
protected:
    StaticArray<ShaderCompilationMessage> _messages;
    bool _result;
};

class IShaderCompiler {
public:
    enum ShaderType {
        ST_Vertex = 0x8B31,
        ST_Fragment = 0x8B30,
        ST_Compute = 0x91B9,
        ST_TessControl = 0x8E88,
        ST_TessEvaluation = 0x8E87,
        ST_Geometry = 0x8DD9
    };

    /// Should call IShaderCompiler::_Optimize
    /// gpu_handle is OpenGL shader object id
    virtual ShaderCompilationOutput Compile(const std::string& code, const ShaderType& type, const unsigned int& gpu_handle) = 0;

    /// gpu_handles contain OpenGL shader object ids
    /// gpu_program_handle is OpenGL shader program object id
    /// byteCode returns status of getting byte code from program
    virtual ShaderCompilationOutput Link(StaticArray<unsigned int> gpu_handles, const unsigned int& gpu_program_handle) = 0;
    virtual ShaderCompilationOutput LinkToByteCode(StaticArray<unsigned int> gpu_handles, const unsigned int& gpu_program_handle, int* outLength, unsigned int* outFromat, void** outData, bool* byteCode) = 0;

    /// IShaderCompiler::Compile calls this method before compilation
    virtual std::string _Optimize(const std::string& code, const ShaderType& type) = 0;
};

class ShaderCompiler : public IShaderCompiler {
public:
    ShaderCompilationOutput Compile(const std::string& code, const ShaderType& type, const unsigned int& gpu_handle) override;
    ShaderCompilationOutput Link(StaticArray<unsigned int> gpu_handles, const unsigned int& gpu_program_handle) override;
    ShaderCompilationOutput LinkToByteCode(StaticArray<unsigned int> gpu_handles, const unsigned int& gpu_program_handle, int* outLength, unsigned int* outFromat, void** outData, bool* byteCode) override;
    std::string _Optimize(const std::string& code, const ShaderType& type) override;
    bool debug_log = true;
};

}

#endif // _MR_SHADER_COMPILER_H_
