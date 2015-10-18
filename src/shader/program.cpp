#include "mr/shader/program.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/shader/shader.hpp"
#include "mr/log.hpp"
#include "mr/buffer/buffer.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

ShaderProgram::ShaderProgram() : _id(0) {
}

ShaderProgram::~ShaderProgram() {}

ShaderProgramPtr ShaderProgram::Create(std::vector<ShaderPtr> const& shaders) {
    ShaderProgramPtr prog = ShaderProgramPtr(new ShaderProgram());
    prog->_id = glCreateProgram();
    if(!shaders.empty()) return (prog->Link(shaders)) ? prog : nullptr;
    return prog;
}

bool ShaderProgram::Link(std::vector<ShaderPtr> const& shaders) {
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
        glGetProgramInfoLog(_id, bufflen, (int*)0, buf);
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
    glProgramUniformMatrix4fv(_id, glGetUniformLocation(_id, name.data()), 1, false, &mat[0][0]);
    return true;
}

bool ShaderProgram::UniformBuffer(std::string const& name, BufferPtr const& buffer, uint32_t buffer_binding) {
    glBindBufferBase(GL_UNIFORM_BUFFER, buffer_binding, buffer->GetId());
    glUniformBlockBinding(_id, glGetUniformBlockIndex(_id, name.data()), buffer_binding);
    return true;
}

}
