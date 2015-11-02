#include "mr/shader/program.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/shader/shader.hpp"
#include "mr/log.hpp"
#include "mr/buffer.hpp"
#include "src/mp.hpp"
#include "src/statecache.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

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
        MR_LOG("Program link output:");
        MR_LOG(buf);
    }

    //get result
    int compile_status = -1;
    glGetProgramiv(_id, GL_LINK_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        MR_LOG("Program link failed");
        return false;
    }

    return true;
}

bool ShaderProgram::UniformMat4(std::string const& name, glm::mat4& mat) {
    MP_BeginSample(ShaderProgram::UniformMat4);

    glProgramUniformMatrix4fv(_id, glGetUniformLocation(_id, name.data()), 1, false, &mat[0][0]);

    MP_EndSample();
    return true;
}

bool ShaderProgram::UniformBuffer(std::string const& name, BufferPtr const& buffer, uint32_t buffer_binding) {
    MP_BeginSample(ShaderProgram::UniformBuffer);

    glBindBufferBase(GL_UNIFORM_BUFFER, buffer_binding, buffer->GetId());
    glUniformBlockBinding(_id, glGetUniformBlockIndex(_id, name.data()), buffer_binding);

    MP_EndSample();
    return true;
}

bool ShaderProgram::UniformFloat(std::string const& name, float value) {
    MP_BeginSample(ShaderProgram::UniformFloat);

    glProgramUniform1f(_id, glGetUniformLocation(_id, name.data()), value);

    MP_EndSample();
    return true;
}

void ShaderProgram::Use() {
    MP_BeginSample(ShaderProgram::Use);

    if(StateCache::Get()->SetShaderProgram(_id)) {
        glUseProgram(_id);
    }

    MP_EndSample();
}

}
