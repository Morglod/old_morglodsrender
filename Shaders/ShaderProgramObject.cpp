#include "ShaderProgramObject.hpp"
#include "ShaderUniforms.hpp"
#include "../Utils/Log.hpp"
#include "ShaderConfig.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace mr {

bool ShaderProgram::Create() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        mr::Log::LogString("Failed ShaderProgram::Create(). Shader program already created.", MR_LOG_LEVEL_WARNING);
        return false;
    }

    handle = glCreateProgram();
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(&gl_str, 0)) {
        mr::Log::LogString("Failed ShaderProgram::Create(). Failed creating OpenGL shader program. " + gl_str, MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif
    SetGPUHandle(handle);
    return true;
}

void ShaderProgram::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteProgram(handle);
        SetGPUHandle(0);
    }
}

ShaderProgramCache ShaderProgram::GetCache() {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) return ShaderProgramCache();

    int bin_length = 0, _a = 0;
    unsigned int form = 0;
    glGetProgramiv(handle, GL_PROGRAM_BINARY_LENGTH, &bin_length);
    unsigned char * buf = new unsigned char[bin_length];

    glGetProgramBinary(handle, bin_length, &_a, &form, &buf[0]);
    return ShaderProgramCache(form, mu::ArrayHandle<unsigned char>(&buf[0], bin_length, true));
}

ShaderProgram::ShaderProgram() : _linked(false), _uniformMap(static_cast<IShaderProgram*>(this)) {
}

ShaderProgram::~ShaderProgram() {
}

}
