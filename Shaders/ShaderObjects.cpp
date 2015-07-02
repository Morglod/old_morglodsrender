#include "ShaderObjects.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Debug.hpp"
#include "../Utils/Log.hpp"
#include "ShaderConfig.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace mr {

/** SHADER CLASS **/

bool Shader::Create(ShaderType const& type) {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        int gl_er = 0;
        mr::gl::ClearError();
#endif
         handle = glCreateShader(type);
         SetGPUHandle(handle);
#ifdef MR_CHECK_LARGE_GL_ERRORS
        if(mr::gl::CheckError(0, &gl_er)) {
            std::string err_str = "Error in Shader::Compile : glCreateShader ended with \"" + std::to_string(gl_er) + "\" code. ";
            switch(gl_er) {
            case GL_INVALID_ENUM:
                err_str += "shaderType is not an accepted value."; break;
            default:
                err_str += "Unknow code."; break;
            }
            mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
            return false;
        }
#endif
    } else {
        mr::Log::LogString("Shader already created.", MR_LOG_LEVEL_WARNING);
    }

    _compiled = false;
    _type = type;
    return true;
}

void Shader::Destroy() {
    _compiled = false;
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteShader(handle);
        SetGPUHandle(0);
    }
}

Shader::Shader() : _type(ShaderType::None), _compiled(false) {
}

Shader::~Shader() {
}

}
