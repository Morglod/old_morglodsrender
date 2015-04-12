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

IShaderUniform* ShaderProgram::CreateUniform(const std::string& name, const mr::IShaderUniform::Type& type, void* value) {
    ShaderUniform* su = new ShaderUniform(name, type, value, dynamic_cast<IShaderProgram*>(this));
    _shaderUniforms.emplace(name, su);
    return su;
}

void ShaderProgram::SetUniform(const std::string& name, const int& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniform1i(handle, glGetUniformLocation(handle, name.c_str()), value);
        else glUniform1i(glGetUniformLocation(handle, name.c_str()), value);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const float& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniform1f(handle, glGetUniformLocation(handle, name.c_str()), value);
        else glUniform1f(glGetUniformLocation(handle, name.c_str()), value);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec2& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniform2fv(handle, glGetUniformLocation(handle, name.c_str()), 2, &value.x);
        else glUniform2fv(glGetUniformLocation(handle, name.c_str()), 2, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec3& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniform3fv(handle, glGetUniformLocation(handle, name.c_str()), 3, &value.x);
        else glUniform3fv(glGetUniformLocation(handle, name.c_str()), 3, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec4& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniform4fv(handle, glGetUniformLocation(handle, name.c_str()), 4, &value.x);
        else glUniform4fv(glGetUniformLocation(handle, name.c_str()), 4, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::mat4& value) {
    unsigned int handle = GetGPUHandle();
    if(handle) {
        if(GLEW_EXT_direct_state_access) glProgramUniformMatrix4fv(handle, glGetUniformLocation(handle, name.c_str()), 1,  GL_FALSE, &value[0][0]);
        else glUniformMatrix4fv(glGetUniformLocation(handle, name.c_str()), 1,  GL_FALSE, &value[0][0]);
    }
}

void ShaderProgram::DeleteUniform(IShaderUniform* su) {
    for(auto it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
        if(it->second == su) {
            delete su;
            _shaderUniforms.erase(it);
            return;
        }
    }
}

IShaderUniform* ShaderProgram::FindShaderUniform(const std::string& name) {
    if(_shaderUniforms.count(name) == 0) return nullptr;
    return _shaderUniforms[name];
}

mu::ArrayHandle<IShaderUniform*> ShaderProgram::GetShaderUniforms() {
    IShaderUniform** uniformsAr = new IShaderUniform*[_shaderUniforms.size()];
    size_t i = 0;
    for(auto const& it : _shaderUniforms) {
        uniformsAr[i] = it.second;
        ++i;
    }
    return mu::ArrayHandle<IShaderUniform*>(&uniformsAr[0], _shaderUniforms.size(), true);
}

mu::ArrayHandle<ShaderUniformInfo> ShaderProgram::GetCompiledUniforms() {
    unsigned int handle = GetGPUHandle();
    int act_uniforms = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &act_uniforms);
    if(act_uniforms == 0) return mu::ArrayHandle<ShaderUniformInfo>();

    ShaderUniformInfo* uni = new ShaderUniformInfo[act_uniforms];

    for(int iu = 0; iu < act_uniforms; ++iu){
        char namebuffer[1024];
        int real_buf_size = 0;
        int unif_size = 0;
        unsigned int uni_type = 0;
        glGetActiveUniform(handle, iu, 1024, &real_buf_size, &unif_size, &uni_type, &namebuffer[0]);
        uni[iu] = ShaderUniformInfo(dynamic_cast<IShaderProgram*>(this), std::string(namebuffer), unif_size, uni_type);
    }

    return mu::ArrayHandle<ShaderUniformInfo>(&uni[0], act_uniforms, false);
}

bool ShaderProgram::IsUniform(std::string const& uniformName) {
    return (_shaderUniforms.count(uniformName) != 0);
}

void ShaderProgram::UpdateUniforms() {
    for(auto const& it : _shaderUniforms) {
        it.second->Update();
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

ShaderProgram::ShaderProgram() : _shaderUniforms(), _linked(false) {
}

ShaderProgram::~ShaderProgram() {
}

}
