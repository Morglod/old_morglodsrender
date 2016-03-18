#include "mr/shader/program.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/shader/shader.hpp"
#include "mr/log.hpp"
#include "mr/buffer.hpp"
#include "src/mp.hpp"
#include "src/statecache.hpp"
#include "mr/shader/ubo.hpp"
#include "mr/alloc.hpp"

#include "mr/pre/glew.hpp"

namespace {

std::unordered_set<mr::ShaderProgram*> _registeredPrograms;

}

namespace mr {

void ShaderProgram::sUBOList::Resize(uint32_t num_) {
    MP_ScopeSample(ShaderProgram::sUBOList::Resize);
    Free();
    arr = MR_NEW_ARRAY(UboInfo, num_);
    num = num_;
}

void ShaderProgram::sUBOList::Free() {
    MP_ScopeSample(ShaderProgram::sUBOList::Free);
    if(arr != nullptr) {
        delete [] arr;
        arr = nullptr;
    }
    num = 0;
}

ShaderProgram::sUBOList::~sUBOList() {
    Free();
}

void ShaderProgram::Destroy() {
    if(_id != 0) {
        glDeleteProgram(_id);
        _id = 0;
    }
}

ShaderProgram::ShaderProgram() : _id(0) {
    _registeredPrograms.insert(this);
}

ShaderProgram::~ShaderProgram() {
    _registeredPrograms.erase(this);
    Destroy();
}

ShaderProgramPtr ShaderProgram::Create(std::vector<ShaderPtr> const& shaders) {
    MP_ScopeSample(ShaderProgram::Create);

    ShaderProgramPtr prog = ShaderProgramPtr(MR_NEW(ShaderProgram));
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

    return _ResetUniforms();
}


bool ShaderProgram::_ResetUniforms() {
    MP_ScopeSample(ShaderProgram::_ResetUniforms);

    _uniformLocations.clear();

    std::vector<std::pair<std::string, UniformBufferDeclPtr>> ubos;
    if(!UniformBufferDecl::Create(this, ubos)) {
        MR_LOG_ERROR(ShaderProgram::_ResetUniforms, "failed get ubo list");
        return false;
    }

    if(ubos.size() != 0) {
        _ubo.Resize(ubos.size());
        for(uint32_t i = 0, n = ubos.size(); i < n; ++i) {
            _ubo.arr[i].name = ubos[i].first;
            _ubo.arr[i].ubo = UniformBuffer::Create(ubos[i].second);
        }
    }

    int32_t uniformsNum = 0;
    glGetProgramiv(_id, GL_ACTIVE_UNIFORMS, &uniformsNum);
    for(int32_t i = 0; i < uniformsNum; ++i) {
        const int32_t nameBufLen = 128;
        char nameBuf[nameBufLen];
        int32_t nameLen = 0;
        int32_t _uniform_size = 0;
        uint32_t _uniform_type = 0;
        glGetActiveUniform(_id, i, nameBufLen, &nameLen, &_uniform_size, &_uniform_type, &nameBuf[0]);
        std::string uniformName(nameBuf, nameLen);

        int32_t uniformLocation = glGetUniformLocation(_id, nameBuf);
        if(uniformLocation == -1) {
            MR_LOG_ERROR(ShaderProgram::_ResetUniforms, "Failed get uniform location \""+uniformName+"\"");
        }
        _uniformLocations[uniformName] = uniformLocation;
    }

    return true;
}

bool ShaderProgram::SetUniformBuffer(std::string const& name, UniformBufferPtr const& ubo) {
    MP_ScopeSample(ShaderProgram::SetUniformBuffer);

    if(_ubo.num == 0) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "there is no UniformBuffers");
        return false;
    }
    for(uint32_t i = 0, n = _ubo.num; i < n; ++i) {
        if(_ubo.arr[i].name == name) {
            if(ubo != nullptr && !_ubo.arr[i].ubo->Match(ubo->GetDecl())) {
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
    MP_ScopeSample(ShaderProgram::SetUniformBuffer);

    if(_ubo.num == 0) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "there is no UniformBuffers");
        return false;
    }

    if(ubo != nullptr && !_ubo.arr[arrayIndex].ubo->Match(ubo->GetDecl())) {
        MR_LOG_ERROR(ShaderProgram::SetUniformBuffer, "failed set UniformBuffer, that not equal to UniformBufferDecl of ShaderProgram's by array index \""+std::to_string(arrayIndex)+"\"");
        return false;
    }
    _ubo.arr[arrayIndex].ubo = ubo;
    return true;
}

void ShaderProgram::_BindUniformBuffer(uint32_t index, uint32_t buffer) {
    MP_ScopeSample(ShaderProgram::_BindUniformBuffer);

    if(StateCache::Get()->SetUniformBuffer(buffer, index)) {
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer);
        glUniformBlockBinding(_id, index, index);
    }
}

bool ShaderProgram::GetUniformBuffer(std::string const& name, ShaderProgram::UboInfo& out_ubo) {
    MP_ScopeSample(ShaderProgram::GetUniformBuffer);

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

bool ShaderProgram::GetUniformBuffer(uint32_t arrayIndex, ShaderProgram::UboInfo& out_ubo) const {
    MP_ScopeSample(ShaderProgram::GetUniformBuffer);

    if(arrayIndex >= _ubo.num) {
        MR_LOG_ERROR(ShaderProgram::GetUniformBuffer, "invalid array index");
        return false;
    }
    //if(_ubo.arr[arrayIndex].ubo == nullptr)
    //    MR_LOG_WARNING(ShaderProgram::GetUniformBuffer, "invalid UniformBufferDecl on UniformBuffer by arrayIndex "+std::to_string(arrayIndex));
    out_ubo = _ubo.arr[arrayIndex];
    return true;
}

void ShaderProgram::Use(ShaderProgramPtr const& program) {
    MP_ScopeSample(ShaderProgram::Use);

    const auto handle = (program != nullptr) ? program->GetId() : 0;
    if(StateCache::Get()->SetShaderProgram(handle)) {
        glUseProgram(handle);
        for(uint32_t i = 0, n = program->_ubo.num; i < n; ++i) {
            const auto ubo = program->_ubo.arr[i].ubo;
            uint32_t bufferId = 0;
            if(ubo != nullptr) bufferId = ubo->GetBuffer()->GetId();
            program->_BindUniformBuffer(i, bufferId);
        }
    }
}

void* ShaderProgram::FoundUniform::GetPtr() const {
    return ubo->At(arrayIndex).ptr;
}

bool ShaderProgram::FindUniform(std::string const& uniformName, FoundUniform& out_uniform) const {
    out_uniform.uniformName = uniformName;
    UniformBufferDecl::Uniform foundUniform;
    for(int i = 0, n = _ubo.num; i < n; ++i) {
        if(_ubo.arr[i].ubo->GetDecl()->FindUniformByName(uniformName, foundUniform, out_uniform.arrayIndex)) {
            out_uniform.ubo = _ubo.arr[i].ubo;
            out_uniform.size = foundUniform.size;
            return true;
        }
    }
    return false;
}

std::unordered_set<ShaderProgram*> ShaderProgram::GetRegisteredPrograms() {
    return _registeredPrograms;
}

}
