#include "Shader.h"

//SUB SHADER
void MR::SubShader::Compile(const char* code, GLenum shader_type){
    if(this->gl_SHADER != 0) glDeleteObjectARB(this->gl_SHADER);
    this->gl_SHADER_TYPE = shader_type;
    this->gl_SHADER = glCreateShaderObjectARB(shader_type);
    glShaderSourceARB(this->gl_SHADER, 1, &code, NULL);
    glCompileShaderARB(this->gl_SHADER);
}

MR::SubShader::SubShader(const char* code, GLenum shader_type){
    this->Compile(code, shader_type);
}

MR::SubShader::~SubShader(){
    glDeleteObjectARB(this->gl_SHADER);
}

//SHADER
void MR::Shader::Link(SubShader** sub_shaders, unsigned int num){
    if(this->gl_PROGRAM != 0) glDeleteObjectARB(this->gl_PROGRAM);
    this->gl_PROGRAM = glCreateProgramObjectARB();
    for(unsigned int i = 0; i < num; ++i){
        glAttachObjectARB(this->gl_PROGRAM, sub_shaders[i]->Get());
    }
    glLinkProgramARB(this->gl_PROGRAM);
}

MR::Shader::Shader(SubShader** sub_shaders, unsigned int num){
    this->Link(sub_shaders, num);
}

MR::Shader::~Shader(){
    glDeleteObjectARB(this->gl_PROGRAM);
}
