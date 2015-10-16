#include "mr/shader/shader.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/log.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

Shader::Shader() : _id(0) {}

std::future<ShaderPtr> Shader::Create(ShaderType const& type, std::string const& code) {
    if(Core::IsWorkerThread()) {
        std::promise<ShaderPtr> promise;
        auto futur = promise.get_future();
        ShaderPtr sh = ShaderPtr(new Shader());
        if(!Shader::_Create(sh.get(), (const uint32_t)type, code))
            promise.set_value(nullptr);
        else
            promise.set_value(sh);
        return futur;
    }
    else {
        PromiseData<ShaderPtr>* pdata = new PromiseData<ShaderPtr>();
        auto fut = pdata->promise.get_future();
        const uint32_t stype = (const uint32_t)type;

        Core::Exec([code, stype](void* arg) -> uint8_t{
            PromiseData<ShaderPtr>* parg = (PromiseData<ShaderPtr>*)arg;
            PromiseData<ShaderPtr>::Ptr free_guard(parg);

            ShaderPtr shader = ShaderPtr(new Shader());
            if(!Shader::_Create(shader.get(), stype, code)) {
                parg->promise.set_value(nullptr);
                return 1;
            }
            parg->promise.set_value(shader);
            return 0;
        }, pdata);

        return fut;
    }
}

bool Shader::_Create(Shader* shader, uint32_t const& type, std::string const& code) {
    shader->_id = glCreateShader(type);
    return _Compile(shader, code);
}

bool Shader::_Compile(Shader* shader, std::string const& code) {
    const char* code_s = code.data();
    int length = code.size();
    glShaderSource(shader->_id, 1, &code_s, &length);
    glCompileShader(shader->_id);

    //get log
    std::string logString = "";
    int bufflen = 0;
    glGetShaderiv(shader->_id, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        logString.resize((size_t)bufflen + 1);
        glGetShaderInfoLog(shader->_id, bufflen, 0, &logString[0]);
        MR_LOG("Shader compilation output: "+logString);
    }

    //get result
    int compile_status = -1;
    glGetShaderiv(shader->_id, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        MR_LOG("Shader compilation failed");
    }

    return true;
}

}
