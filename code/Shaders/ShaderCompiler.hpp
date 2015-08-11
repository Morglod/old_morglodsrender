#pragma once

#ifndef _MR_SHADER_COMPILER_H_
#define _MR_SHADER_COMPILER_H_

#include "ShaderInterfaces.hpp"

#include <mu/Containers.hpp>
#include <mu/Singleton.hpp>

#include <string>

/**
    Class used to compile shader's code, optimization and errors processing.
*/

namespace mr {

struct ShaderCompilationMessage {
public:
    enum MessageType { MT_Info, MT_Warning, MT_Error };
    static std::string MessageTypeNames[3];

    inline MessageType GetType() { return _type; }
    inline int GetCode() { return _code; }
    inline const std::string& GetText() { return _text; }
    inline const unsigned int& GetLine() { return _line; }

    ShaderCompilationMessage() {}
    ShaderCompilationMessage(const MessageType& type,
                             const int& code,
                             const unsigned int& line,
                             const std::string& text);
    ShaderCompilationMessage& operator = (const ShaderCompilationMessage&);

protected:
    MessageType _type = MT_Info;
    int _code = 0;
    unsigned int _line = 0;
    std::string _text = "";
};

class ShaderCompilationOutput {
public:
    inline mu::ArrayHandle<ShaderCompilationMessage>& GetMessages() { return _messages; }
    inline ShaderCompilationMessage& GetMessage(const size_t& i) { return _messages.GetArray()[i]; }
    inline size_t GetMessagesNum() { return _messages.GetNum(); }
    inline bool Good() { return _result; }

    /// By default result is false
    ShaderCompilationOutput();

    /// Memory, allocated for array, will be free on ShaderCompilationOutput deletion
    ShaderCompilationOutput(ShaderCompilationMessage* ar, const size_t& num, const bool& result);

    ShaderCompilationOutput(mu::ArrayHandle<ShaderCompilationMessage> const& msg, const bool& result);
protected:
    mu::ArrayHandle<ShaderCompilationMessage> _messages;
    bool _result;
};

class IShaderCompiler {
public:
    virtual void Release() = 0;

    /// Should call IShaderCompiler::_Optimize
    virtual ShaderCompilationOutput Compile(IShader* shaderObject, std::string const& code) = 0;

    virtual ShaderCompilationOutput Link(IShaderProgram* shaderProgramObject, mu::ArrayHandle<IShader*> shaderObjects) = 0;

    /// byteCode returns status of getting byte code from program
    virtual ShaderCompilationOutput LinkToByteCode(IShaderProgram* shaderProgramObject, mu::ArrayHandle<IShader*> shaderObjects, int* __restrict__ outLength, unsigned int* __restrict__ outFromat, void** __restrict__ outData, bool* __restrict__ byteCode) = 0;

    /// IShaderCompiler::Compile calls this methods before compilation
    virtual std::string _Optimize(ShaderType const& type, std::string const& code) = 0;
    //virtual std::string PreProcess(std::string code, ShaderType const& type, std::string const& shaderDirectory) = 0;

    virtual ~IShaderCompiler() {}
};

class ShaderCompiler : public IShaderCompiler, public mu::Singleton<ShaderCompiler> {
public:
    void Release() override;
    ShaderCompilationOutput Compile(IShader* shaderObject, std::string const& code) override;
    ShaderCompilationOutput Link(IShaderProgram* shaderProgramObject, mu::ArrayHandle<IShader*> shaderObjects) override;
    ShaderCompilationOutput LinkToByteCode(IShaderProgram* shaderProgramObject, mu::ArrayHandle<IShader*> shaderObjects, int* outLength, unsigned int* outFromat, void** outData, bool* byteCode) override;
    std::string _Optimize(ShaderType const& type, std::string const& code) override;
    //std::string PreProcess(std::string code, ShaderType const& type, std::string const& shaderDirectory) override;
    bool debug_log = true;
    virtual ~ShaderCompiler();
};

}

#endif // _MR_SHADER_COMPILER_H_
