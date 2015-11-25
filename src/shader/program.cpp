#include "mr/shader/program.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/shader/shader.hpp"
#include "mr/log.hpp"
#include "mr/buffer.hpp"
#include "src/mp.hpp"
#include "src/statecache.hpp"
#include "mr/shader/ubo.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

void ShaderProgram::sUBOList::Resize(uint32_t num_) {
    Free();
    arr = new UBO[num_];
    num = num_;
}

void ShaderProgram::sUBOList::Free() {
    if(arr != nullptr) {
        delete [] arr;
        arr = nullptr;
    }
    num = 0;
}

ShaderProgram::sUBOList::~sUBOList() {
    Free();
}

ShaderProgram::ShaderProgram() : _id(0) {
}

ShaderProgram::~ShaderProgram() {}

ShaderProgramPtr ShaderProgram::Create(std::vector<ShaderPtr> const& shaders) {
    MP_ScopeSample(ShaderProgram::Create);

    ShaderProgramPtr prog = ShaderProgramPtr(new ShaderProgram());
    prog->_id = glCreateProgram();
    if(!shaders.empty()) return (prog->Link(shaders)) ? prog : nullptr;
    return prog;
}

bool ShaderProgram::Link(std::vector<ShaderPtr> const& shaders) {
    MP_ScopeSample(ShaderProgram::Link);

    if(shaders.empty()) return false;
    for(size_t i = 0, n = shaders.size(); i < n; ++i) {
        glAttachShader(_id, shaders[i]->GetId());
    }
    glLinkProgram(_id);

    //get log
    int bufflen = 0;
    glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        char buf[bufflen];
        glGetProgramInfoLog(_id, bufflen, nullptr, buf);
        MR_LOG("Program link output: \n");
        MR_LOG(buf);
    }

    //get result
    int compile_status = -1;
    glGetProgramiv(_id, GL_LINK_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        MR_LOG_ERROR(ShaderProgram::Link, "Program link failed");
        return false;
    }

    return _InitUBO();
}


bool ShaderProgram::_InitUBO() {
    MP_ScopeSample(ShaderProgram::_InitUBO);

    std::vector<std::pair<std::string, UniformBufferDeclPtr>> ubos;
    if(!UniformBufferDecl::Create(this, ubos)) {
        MR_LOG_ERROR(ShaderProgram::_InitUBO, "failed get ubo list");
        return false;
    }

    if(ubos.size() != 0) {
        _uniforms.Resize(ubos.size());
        for(uint32_t i = 0, n = 0; i < n; ++i) {
            _uniforms.arr[i].name = ubos[i].first;
            _uniforms.arr[i].decl = ubos[i].second;
        }
    }

    return true;
}

/*
bool ShaderProgram::UniformBuffer(std::string const& name, BufferPtr const& buffer, uint32_t buffer_binding) {
    MP_BeginSample(ShaderProgram::UniformBuffer);

    const auto buffer_id = buffer->GetId();
    if(StateCache::Get()->SetUniformBuffer(buffer_id, buffer_binding)) {
        glBindBufferBase(GL_UNIFORM_BUFFER, buffer_binding, buffer_id);
    }
    glUniformBlockBinding(_id, glGetUniformBlockIndex(_id, name.data()), buffer_binding);

    MP_EndSample();
    return true;
}
*/
bool ShaderProgram::_UniformFloat(std::string const& name, float value) {
    MP_BeginSample(ShaderProgram::UniformFloat);

    const auto location = glGetUniformLocation(_id, name.data());
    if(location == -1)
        MR_LOG_WARNING(ShaderProgram::UniformMat4, "failed get \""+name+"\"'s location");
    else
        glProgramUniform1f(_id, location, value);

    MP_EndSample();
    return true;
}

bool ShaderProgram::_UniformMat4(std::string const& name, glm::mat4& mat) {
    MP_BeginSample(ShaderProgram::UniformMat4);

    const auto location = glGetUniformLocation(_id, name.data());
    if(location == -1)
        MR_LOG_WARNING(ShaderProgram::UniformMat4, "failed get \""+name+"\"'s location");
    else
        glProgramUniformMatrix4fv(_id, location, 1, false, &mat[0][0]);

    MP_EndSample();
    return true;
}

bool ShaderProgram::SetUniformBuffer(std::string const& name, UniformBufferPtr const& ubo) {
    if(_ubo.num == 0) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "there is no UniformBuffers");
        return false;
    }
    for(uint32_t i = 0, n = _ubo.num; i < n; ++i) {
        if(_ubo.arr[i].name == name) {
            if(ubo != nullptr && !_ubo.arr[i].decl->Equal(ubo->GetDecl())) {
                MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "failed set UniformBuffer, that not equal to UniformBufferDecl of ShaderProgram's by name \""+name+"\"");
                return false;
            }
            _ubo.arr[i].ubo = ubo;
            return true;
        }
    }
    MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "failed find UniformBuffer by name \""+name+"\"");
    return false;
}

bool ShaderProgram::SetUniformBuffer(uint32_t arrayIndex, UniformBufferPtr const& ubo) {
    if(_ubo.num == 0) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "there is no UniformBuffers");
        return false;
    }

    if(ubo != nullptr && !_ubo.arr[arrayIndex].decl->Equal(ubo->GetDecl())) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "failed set UniformBuffer, that not equal to UniformBufferDecl of ShaderProgram's by array index \""+std::to_string(arrayIndex)+"\"");
        return false;
    }
    _ubo.arr[arrayIndex].ubo = ubo;
    return true;
}

bool ShaderProgram::GetUniformBuffer(std::string const& name, ShaderProgram::UBO& out_ubo) {
    if(_ubo.num == 0) {
        MR_LOG_ERROR(ShaderProgram::GetUniformBuffer, "there is no UniformBuffers");
        return false;
    }
    for(uint32_t i = 0, n = _ubo.num; i < n; ++i) {
        if(_ubo.arr[i].name == name) {
            out_ubo = _ubo.arr[i];
            return true;
        }
    }
    MR_LOG_ERROR(ShaderProgram::GetUniformBuffer, "failed find UniformBuffer by name \""+name+"\"");
    return false;
}

bool ShaderProgram::GetUniformBuffer(uint32_t arrayIndex, ShaderProgram::UBO& out_ubo) const {
    if(arrayIndex >= _ubo.num) {
        MR_LOG_ERROR(ShaderProgram::GetUniformBuffer, "invalid array index");
        return false;
    }
    if(_ubo.arr[arrayIndex].decl == nullptr)
        MR_LOG_WARNING(ShaderProgram::GetUniformBuffer, "invalid UniformBufferDecl on UniformBuffer by arrayIndex "+std::to_string(arrayIndex));
    out_ubo = _ubo.arr[arrayIndex];
    return true;
}

void ShaderProgram::Use(ShaderProgramPtr const& program) {
    MP_BeginSample(ShaderProgram::Use);

    const auto handle = (program != nullptr) program->GetId() : 0;
    if(StateCache::Get()->SetShaderProgram(handle)) {
        glUseProgram(handle);
    }

    MP_EndSample();
}

}
