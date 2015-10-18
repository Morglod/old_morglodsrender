#include "mr/shader/shader.hpp"
#include "mr/core.hpp"
#include "src/thread/util.hpp"
#include "mr/log.hpp"

#include "mr/pre/glew.hpp"

namespace mr {

Shader::Shader() : _id(0), _type(ShaderType::None) {}
Shader::~Shader() {}

ShaderPtr Shader::Create(ShaderType const& type, std::string const& code) {
    ShaderPtr shader = ShaderPtr(new Shader());
    shader->_id = glCreateShader((uint32_t)type);
    shader->_type = type;
    if(!code.empty()) return (shader->Compile(code) ? shader : nullptr);
    return shader;
}

bool Shader::Compile(std::string const& code) {
    const char* code_s = code.data();
    int length = code.size();
    glShaderSource(_id, 1, &code_s, &length);
    glCompileShader(_id);

    //get log
    std::string logString = "";
    int bufflen = 0;
    glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        logString.resize((size_t)bufflen + 1);
        glGetShaderInfoLog(_id, bufflen, nullptr, &logString[0]);
        MR_LOG("Shader compilation output: "+logString);
    }

    //get result
    int compile_status = -1;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &compile_status);
    if(compile_status != GL_TRUE){
        MR_LOG("Shader compilation failed");
        return false;
    }

    return true;
}

}
