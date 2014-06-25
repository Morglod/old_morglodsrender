#include "ShaderUniforms.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Log.hpp"
#include "../Utils/Debug.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

bool ShaderUniform::Map(IShaderProgram* program) {
    AssertExec(!program, MR::Log::LogString("Error in ShaderUniform::Map : program == nullptr.", MR_LOG_LEVEL_ERROR))

    MR::MachineInfo::ClearError();
    int gl_er = 0;

    _gpu_location = glGetUniformLocation(program->GetGPUHandle(), _name.c_str());

#ifdef MR_CHECK_SMALL_GL_ERRORS
    if(MR::MachineInfo::CatchError(0, &gl_er)) {
        std::string err_str = "Error in ShaderUniform::Map : glGetUniformLocationARB ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "Shader program id is not generated by OpenGL values."; break;
        case GL_INVALID_OPERATION:
            err_str += "id is not an OpenGL program object or program is not successfully linked."; break;
        default:
            err_str += "Unknow code."; break;
        }
        MR::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif

    if(_gpu_location == -1) {
#ifdef MR_CHECK_SMALL_GL_ERRORS
        MR::Log::LogString("Error in ShaderUniform::Map : _gpu_location == -1.", MR_LOG_LEVEL_ERROR);
#endif
        return false;
    }

    OnMapped(this, program, _gpu_location);

    return true;
}

ShaderUniform::ShaderUniform
(const std::string& name, const ShaderUniform::Type& type, void* value, IShaderProgram* program)
: _name(name), _gpu_location(-1), _type(type), _value(value) {
    Map(program);
}

}