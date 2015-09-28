#include "mr/shader/program.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/shader/shader.hpp"
#include "mr/log.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

std::future<ShaderProgramPtr> ShaderProgram::Create(std::vector<ShaderPtr> const& shaders) {
    PromiseData<ShaderProgramPtr>* pdata = new PromiseData<ShaderProgramPtr>();
    auto fut = pdata->promise.get_future();

    Core::Exec([shaders](void* arg){
        PromiseData<ShaderProgramPtr>* parg = (PromiseData<ShaderProgramPtr>*)arg;
        PromiseData<ShaderProgramPtr>::Ptr free_guard(parg);

        ShaderProgram* prog = new ShaderProgram();
        if(!ShaderProgram::_Create(prog, shaders)) {
            parg->promise.set_value(nullptr);
            return;
        }
        parg->promise.set_value(ShaderProgramPtr(prog));
    }, pdata);

    return fut;
}

std::future<bool> ShaderProgram::UniformMat4(std::string const& name, glm::mat4 const& mat) {
    PromiseData<bool>* pdata = new PromiseData<bool>();
    auto fut = pdata->promise.get_future();

    ShaderProgram* sprog = this;
    Core::Exec([sprog, name, mat](void* arg){
        PromiseData<bool>* parg = (PromiseData<bool>*)arg;
        PromiseData<bool>::Ptr free_guard(parg);
        parg->promise.set_value(ShaderProgram::_UniformMat4(sprog, name, mat));
    }, pdata);

    return fut;
}

ShaderProgram::ShaderProgram() : _id(0) {
}

bool ShaderProgram::_Create(ShaderProgram* prog, std::vector<ShaderPtr> const& shaders) {
    prog->_id = glCreateProgram();
    return _Link(prog, shaders);
}

bool ShaderProgram::_Link(ShaderProgram* prog, std::vector<ShaderPtr> const& shaders) {
    for(size_t i = 0, n = shaders.size(); i < n; ++i) {
        glAttachShader(prog->_id, shaders[i]->GetId());
    }
    glLinkProgram(prog->_id);

    //get log
    std::string logString = "";
    int bufflen = 0;
    glGetProgramiv(prog->_id, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        logString.resize((size_t)bufflen + 1);
        glGetProgramInfoLog(prog->_id, bufflen, 0, &logString[0]);
        MR_LOG("Program link output: "+logString);
    }

    //get result
    int compile_status = -1;
    glGetProgramiv(prog->_id, GL_LINK_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        MR_LOG("Program link failed");
    }

    return true;
}

bool ShaderProgram::_UniformMat4(ShaderProgram* prog, std::string const& name, glm::mat4 const& mat_) {
    glm::mat4 mat = mat_;
    glProgramUniformMatrix4fv(prog->_id, glGetUniformLocation(prog->_id, name.data()), 1, false, &mat[0][0]);
    return true;
}

}
