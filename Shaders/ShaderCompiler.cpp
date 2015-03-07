#include "ShaderCompiler.hpp"
#include "../MachineInfo.hpp"
#include "../Config.hpp"
#include "../Utils/Log.hpp"

#include <fstream>

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace mr {

std::string ShaderCompilationMessage::MessageTypeNames[3] { std::string("Info"), std::string("Warning"), std::string("Error") };

ShaderCompilationMessage::ShaderCompilationMessage
(const MessageType& type, const int& code, const unsigned int& line, const std::string& text)
: _type(type), _code(code), _line(line), _text(text) {}

ShaderCompilationMessage& ShaderCompilationMessage::operator = (const ShaderCompilationMessage& msg) {
    if(this == &msg) {
        return *this;
    }

    _type = msg._type;
    _code = msg._code;
    _line = msg._line;
    _text = msg._text;

    return *this;
}

ShaderCompilationOutput::ShaderCompilationOutput() : _messages(), _result(false) {}
ShaderCompilationOutput::ShaderCompilationOutput(ShaderCompilationMessage* ar, const size_t& num, const bool& result) : _messages(ar, num, true), _result(result) {}
ShaderCompilationOutput::ShaderCompilationOutput(const TStaticArray<ShaderCompilationMessage>& msg, const bool& result) : _messages(msg), _result(result) {}

/**
    COMPILE
**/

ShaderCompilationOutput ShaderCompiler::Compile(const std::string& code, const mr::ShaderCompiler::ShaderType& type, const unsigned int& gpu_handle) {
    if((code.size() == 0 ) || (gpu_handle == 0)) {
        return mr::ShaderCompilationOutput (
                                new mr::ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, 0, 0, "(code.size() == 0) || (gpu_handle == 0)" ) },
                                1,
                                false
                                );
    }

    std::string str = _Optimize(code, type);
    const char* c = _Optimize(code, type).c_str();

    //for gl error handling
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    mr::MachineInfo::ClearError();
#endif

    int str_size = str.size();
    glShaderSource(gpu_handle, 1, &c, &str_size);

    //if glShaderSource is failed
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)){
        std::string err_str = "Error in ShaderCompiler::Compile : glShaderSource ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "gpu_handle is not generated by OpenGL value or Shader's code lines num is less than 0."; break;
        case GL_INVALID_OPERATION:
            err_str += "gpu_handle is not OpenGL shader object."; break;
        default:
            err_str += "Unknow code."; break;
        }
        if(debug_log) mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);

        return ShaderCompilationOutput(
                                new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, gl_er, 0, err_str ) },
                                1,
                                false
                                );
    }
#endif

    glCompileShader(gpu_handle);

    //get log
    std::string logString = "";
    int bufflen = 0;
    glGetShaderiv(gpu_handle, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        logString.resize((size_t)bufflen + 1);
        glGetShaderInfoLog(gpu_handle, bufflen, 0, &logString[0]);
        if(debug_log) mr::Log::LogString("Shader compilation output: "+logString, MR_LOG_LEVEL_INFO);
    }

    //get result
    int compile_status = -1;
    glGetShaderiv(gpu_handle, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        mr::Log::LogString("Shader compilation failed", MR_LOG_LEVEL_ERROR);
    }

    //if glCompileShader is failed
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)){
        std::string err_str = "Error in ShaderCompiler::Compile : glCompileShader ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "gpu_handle is not generated by OpenGL value."; break;
        case GL_INVALID_OPERATION:
            err_str += "gpu_handle is not OpenGL shader object."; break;
        default:
            err_str += "Unknow code."; break;
        }
        if(debug_log) mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);

        return ShaderCompilationOutput( new ShaderCompilationMessage [2] { ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, gl_er, 0, err_str ),
                                                                            ShaderCompilationMessage( ShaderCompilationMessage::MT_Info, 0, 0, logString ) },
                                2,
                                false
                                );
    }
#endif

    return ShaderCompilationOutput(
                                (logString != "") ? new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Info, 0, 0, logString ) } : nullptr,
                                (logString != "") ? 1 : 0,
                                (bool)compile_status
                                );
}

/**
    LINK
**/

ShaderCompilationOutput ShaderCompiler::Link(TStaticArray<unsigned int> gpu_handles, const unsigned int& gpu_program_handle) {
    if(gpu_program_handle == 0) {
        return ShaderCompilationOutput(
                                new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, 0, 0, "gpu_program_handle == 0" ) },
                                1,
                                false
                                );
    }

    //for gl error handling
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    mr::MachineInfo::ClearError();
#endif

    //Attach only not attached shaders
    unsigned int attached_shaders[32];
    int real_num = 0;
    glGetAttachedShaders(gpu_program_handle, 32, &real_num, &attached_shaders[0]);

    for(size_t i = 0; i < gpu_handles.GetNum(); ++i) {
        for(unsigned short i2 = 0; i2 < ((unsigned short)real_num); ++i2) {
            if(attached_shaders[i2] == gpu_handles.At(i)) goto __Link__AttachedList__Next;
        }
        glAttachShader(gpu_program_handle, gpu_handles.At(i));
    __Link__AttachedList__Next:
        ;
    }

    //if glAttachShader is failed
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)){
        std::string err_str = "Error in ShaderCompiler::Link : glAttachShader ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "One of gpu_handles or gpu_program_handle is not generated by OpenGL value."; break;
        case GL_INVALID_OPERATION:
            err_str += "gpu_program_handle is not a program object or one of gpu_handles is not a shader object or one of gpu_handles is already attached."; break;
        default:
            err_str += "Unknow code."; break;
        }
        if(debug_log) mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);

        return ShaderCompilationOutput(
                                new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, gl_er, 0, err_str ) },
                                1,
                                false
                                );
    }
#endif

    glProgramParameteri(gpu_program_handle, GL_PROGRAM_BINARY_RETRIEVABLE_HINT, GL_TRUE); ///FOR GetCache
    glLinkProgram(gpu_program_handle);

    //get log
    std::string logString = "";
    int bufflen = 0;
    glGetProgramiv(gpu_program_handle, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        logString.resize(bufflen + 1);
        glGetProgramInfoLog(gpu_program_handle, bufflen, 0, &logString[0]);
        if(debug_log) mr::Log::LogString("Shader program linking output: "+logString, MR_LOG_LEVEL_INFO);
    }

    int link_status = -1;
    glGetProgramiv(gpu_program_handle, GL_LINK_STATUS, &link_status);
    if(link_status != GL_TRUE){
        if(debug_log) mr::Log::LogString("Shader program linking failed", MR_LOG_LEVEL_ERROR);
    }

    //if glLinkProgram is failed
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)){
        std::string err_str = "Error in ShaderCompiler::Link : glLinkProgram ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "gpu_program_handle is not generated by OpenGL value."; break;
        case GL_INVALID_OPERATION:
            err_str += "gpu_program_handle is not a program object or gpu_program_handle is? is the currently active program object and transform feedback mode is active."; break;
        default:
            err_str += "Unknow code."; break;
        }
        if(debug_log) mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);

        return ShaderCompilationOutput(
                                new ShaderCompilationMessage[2]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, gl_er, 0, err_str ),
                                                            ShaderCompilationMessage( ShaderCompilationMessage::MT_Info, 0, 0, logString ) },
                                2,
                                false
                                );
    }
#endif

    /** DEBUG
    int act_uniforms = 0;
    glGetProgramiv(gpu_program_handle, GL_ACTIVE_UNIFORMS, &act_uniforms);

    for(int iu = 0; iu < act_uniforms; ++iu){
        char namebuffer[1024];
        int real_buf_size = 0;
        int unif_size = 0;
        unsigned int uni_type = 0;
        glGetActiveUniform(gpu_program_handle, iu, 1024, &real_buf_size, &unif_size, &uni_type, &namebuffer[0]);

        MR::Log::LogString("Program("+std::to_string(gpu_program_handle)+") Uniform \""+std::string(namebuffer)+"\" sizeof("+std::to_string(unif_size)+") typeof("+std::to_string(uni_type)+")", MR_LOG_LEVEL_WARNING);
    }**/

    return ShaderCompilationOutput(
                                (logString != "") ? new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Info, 0, 0, logString ) } : nullptr,
                                (logString != "") ? 1 : 0,
                                (bool)link_status
                                );
}

/**
    LINK TO BYTE CODE
**/

ShaderCompilationOutput ShaderCompiler::LinkToByteCode(TStaticArray<unsigned int> gpu_handles, const unsigned int & gpu_program_handle, int* outLength, unsigned int* outFromat, void** outData, bool* byteCode) {
    if(outLength == 0 || outFromat == 0 || outData == 0 || byteCode == 0) {
        return ShaderCompilationOutput(
                                new ShaderCompilationMessage[1]{ ShaderCompilationMessage( ShaderCompilationMessage::MT_Error, 0, 0, "outLength == 0 || outFromat == 0 || outData == 0 || byteCode == 0" ) },
                                1,
                                false
                                );
    }

    ShaderCompilationOutput out = Link(gpu_handles, gpu_program_handle);
    if(!out.Good()) return out;

    //for gl error handling
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    mr::MachineInfo::ClearError();
#endif

    int bin_length = 0;
    glGetProgramiv(gpu_program_handle, GL_PROGRAM_BINARY_LENGTH, &bin_length);
    unsigned char * buf = new unsigned char[bin_length];

    glGetProgramBinary(gpu_program_handle, bin_length, outLength, outFromat, &buf[0]);
    *outData = &buf[0];

    //if glGetProgramBinary is failed
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)) {
        std::string err_str = "Error in ShaderCompiler::LinkToByteCode : glGetProgramBinary ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_OPERATION:
            err_str += "bufSize? is less than the size of GL_PROGRAM_BINARY_LENGTH? for program? or shader program (gpu_program_handle) is not linked successfuly."; break;
        default:
            err_str += "Unknow code."; break;
        }
        if(debug_log) mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
        *byteCode = false;
        return out;
    }
#endif

    *byteCode = true;
    return out;
}

/**
    OPTIMIZE
**/

std::string ShaderCompiler::_Optimize(const std::string& code, const ShaderType& type) {
    return code;
}
/*
std::string ShaderCompiler::PreProcess(std::string code, ShaderType const& type, std::string const& shaderDirectory) {
    const std::string include_const("#include");
PreProcess_STEP:
    size_t it_include = code.find("#include");
    const auto it_include_const = it_include;
    if(it_include == std::string::npos) return code;
    std::advance(it_include, include_const.size());
    auto it_file_begin = code.find('"', it_include);
    if(it_file_begin == std::string::npos) {
        code = code.erase(it_include_const, it_include);
        goto PreProcess_STEP;
    }
    auto it_file_end = code.find('"', it_file_begin+1);
    if(it_file_end == std::string::npos) {
        code = code.erase(it_include_const, it_file_begin+1);
        goto PreProcess_STEP;
    }
    std::string fileName = code.substr(it_file_begin+1, it_file_end - it_file_begin);

    std::ifstream include_file(shaderDirectory + "/" + fileName, std::ifstream::in);
    if(!include_file.is_open()) {
        code = code.erase(it_include_const, it_file_end+1);
        goto PreProcess_STEP;
    }

    std::string includeFile = "";
    std::string nextLine = "";
    while(std::getline(include_file, nextLine)) includeFile += nextLine + "\n";
    include_file.close();

    code = code.erase(it_include_const, it_file_end+1);
    code = code.insert(it_include_const, includeFile);

    goto PreProcess_STEP;
}
*/
void ShaderCompiler::Release() {
    if(mr::gl::GetVersionAsFloat() >= 4.1f)
        glReleaseShaderCompiler();
}

ShaderCompiler::~ShaderCompiler() {
}

}
