#include "Shader.hpp"
#include "Log.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <iostream>
#include <fstream>

char *textFileRead(const char *fn, int * count = nullptr) {
    FILE *fp;
    char *content = NULL;

    int icount = 0;

    if (fn != NULL) {
        fp = fopen(fn,"rt");

        if (fp != NULL) {

            fseek(fp, 0, SEEK_END);
            icount = ftell(fp);
            rewind(fp);

            if (icount > 0) {
                content = (char *)malloc(sizeof(char) * (icount+1));
                icount = fread(content,sizeof(char),icount,fp);
                content[icount] = '\0';
            }
            fclose(fp);
        }
    }
    if(count != nullptr) *count = icount;

    return content;
}

void MR::ShaderUniform::MapUniform(const unsigned int & shader_program) {
    _uniform_location = glGetUniformLocationARB(shader_program, _name);
    OnMapped(this, shader_program, _uniform_location);
}

MR::ShaderUniform::ShaderUniform(const char* Name, const ShaderUniformTypes& Type, void* Value, const GLenum& shader_program) : _name(Name), _type(Type), _value(Value) {
    MapUniform(shader_program);
}

//UNIFORM BLOCK

void MR::ShaderUniformBlock::MapBlock(const GLenum& shader_program) {
    _uniform_block_index = glGetUniformBlockIndex(shader_program, _name);
    glGetActiveUniformBlockiv(shader_program, _uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &_block_size);
    glGetUniformIndices(shader_program, _num_uniforms, _uniform_names, _uniform_indecies);
    glGetActiveUniformsiv(shader_program, _num_uniforms, _uniform_indecies, GL_UNIFORM_OFFSET, _uniform_offsets);
}

void MR::ShaderUniformBlock::BufferData(unsigned char* Data) {
    _data = Data;
    glGenBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, _block_size, _data, GL_STREAM_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, _uniform_block_index, _ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MR::ShaderUniformBlock::ChangeBufferedData(const int& ChangedPos, const int& ChangedSize) {
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, ChangedPos, ChangedSize, _data+ChangedPos);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

MR::ShaderUniformBlock::ShaderUniformBlock(const char* Name, const int& NumUniforms, const char** UniformNames, /*const ShaderUniformBlockTypes& Type,*/ const GLenum& shader_program) :
    _name(Name), _data(NULL), _uniform_block_index(0), _block_size(0), _num_uniforms(NumUniforms), _uniform_names(UniformNames), _uniform_indecies( new unsigned int[NumUniforms] ), _uniform_offsets( new int[NumUniforms] ), /*type(Type),*/ _ubo(0) {
    MapBlock(shader_program);
}

MR::ShaderUniformBlock::~ShaderUniformBlock() {
    glDeleteBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//SUB SHADER
void MR::SubShader::Compile(const char* code, const SubShader::Type& shader_type) {
    if(this->_shader != 0) glDeleteObjectARB(this->_shader);
    this->_type = shader_type;
    this->_shader = glCreateShaderObjectARB((unsigned int)shader_type);
    glShaderSourceARB(this->_shader, 1, &code, NULL);
    glCompileShaderARB(this->_shader);

    int bufflen = 0;
    glGetShaderiv(this->_shader, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        GLchar* logString = new GLchar[bufflen + 1];
        glGetShaderInfoLog(this->_shader, bufflen, 0, logString);
        MR::Log::LogString("Sub shader output: "+std::string(logString), MR_LOG_LEVEL_WARNING);

        delete [] logString;
        logString = 0;
    }

    OnCompiled(this, code, shader_type);
}

MR::SubShader::SubShader(const char* code, const SubShader::Type& type) {
    this->Compile(code, type);
}

MR::SubShader::~SubShader() {
    glDeleteObjectARB(this->_shader);
}

MR::SubShader* MR::SubShader::FromFile(const std::string& file, const SubShader::Type& shader_type) {
    return new MR::SubShader(textFileRead(file.c_str()), shader_type);
}

//SHADER
void MR::Shader::Link(SubShader** sub_shaders, const unsigned int& num) {
    if(this->_program != 0) glDeleteObjectARB(this->_program);
    this->_program = glCreateProgramObjectARB();
    for(unsigned int i = 0; i < num; ++i) {
        glAttachObjectARB(this->_program, sub_shaders[i]->Get());
    }
    glLinkProgramARB(this->_program);

    int bufflen = 0;
    glGetProgramiv(this->_program, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        GLchar* logString = new GLchar[bufflen + 1];
        glGetProgramInfoLog(this->_program, bufflen, 0, logString);
        MR::Log::LogString("Shader output: "+std::string(logString), MR_LOG_LEVEL_WARNING);

        delete [] logString;
        logString = 0;
    }
}

void MR::Shader::Link(){
    Link(_sub_shaders.data(), _sub_shaders.size());
}

bool MR::Shader::Load() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source != "") {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Importing shader from ("+this->_source+")", MR_LOG_LEVEL_INFO);
        std::ifstream ffile(this->_source, std::ios::in | std::ios::binary);
        if(!ffile.is_open()) return false;

        int sub_shaders_num = 0;
        ffile.read( reinterpret_cast<char*>(&sub_shaders_num), sizeof(int));
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shaders num "+std::to_string(sub_shaders_num));

        MR::SubShader** subs = new MR::SubShader*[sub_shaders_num];

        for(int i = 0; i < sub_shaders_num; ++i) {
            unsigned char type = 0;
            ffile.read( reinterpret_cast<char*>(&type), sizeof(unsigned char));
            if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shader type "+std::to_string((int)type));

            int chars_num = 0;
            ffile.read( reinterpret_cast<char*>(&chars_num), sizeof(int));
            if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Sub shader code chars num "+std::to_string(chars_num));

            void* p_code = new unsigned char[chars_num];
            ffile.read( reinterpret_cast<char*>(&((unsigned char*)p_code)[0]), sizeof(unsigned char)*chars_num);

            for (int ci = 0; ci < chars_num; ++ci) {
                ((char*)p_code)[ci] = (char)(( (int) ((unsigned char*)p_code)  [ci])-127);
            }

            GLenum sh_type = GL_VERTEX_SHADER;
            switch(type) {
            case 3:
                sh_type = GL_COMPUTE_SHADER;
                break;
            case 1:
                sh_type = GL_VERTEX_SHADER;
                break;
            case 4:
                sh_type = GL_TESS_CONTROL_SHADER;
                break;
            case 5:
                sh_type = GL_TESS_EVALUATION_SHADER;
                break;
            case 6:
                sh_type = GL_GEOMETRY_SHADER;
                break;
            case 2:
                sh_type = GL_FRAGMENT_SHADER;
                break;
            }

            subs[i] = new MR::SubShader((char*)p_code, (SubShader::Type)((unsigned int)sh_type));
        }

        Link(subs, sub_shaders_num);
    } else if(this->_resource_manager->GetDebugMessagesState()) {
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }
    if(this->_program == 0) {
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") loading failed. GL_PROGRAM is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}

void MR::Shader::UnLoad() {
    if(_loaded) {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") unloading", MR_LOG_LEVEL_INFO);
        glDeleteObjectARB(this->_program);
    }
}

void MR::Shader::Use() {
    glUseProgramObjectARB(this->_program);
    for(auto it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
        if((*it)->GetValuePtr() == nullptr) continue;
        if((*it)->GetType() == ShaderUniformTypes::INT) glUniform1i((*it)->GetOpenGLLocation(), ((int*)(*it)->GetValuePtr())[0]);
        if((*it)->GetType() == ShaderUniformTypes::FLOAT) glUniform1f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0]);
        if((*it)->GetType() == ShaderUniformTypes::VEC2) glUniform2f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1]);
        if((*it)->GetType() == ShaderUniformTypes::VEC3) glUniform3f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2]);
        if((*it)->GetType() == ShaderUniformTypes::VEC4) glUniform4f((*it)->GetOpenGLLocation(), ((float*)(*it)->GetValuePtr())[0], ((float*)(*it)->GetValuePtr())[1], ((float*)(*it)->GetValuePtr())[2], ((float*)(*it)->GetValuePtr())[3]);
        if((*it)->GetType() == ShaderUniformTypes::MAT4) {
            glUniformMatrix4fv((*it)->GetOpenGLLocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(*it)->GetValuePtr())[0][0]));
        }
    }
}

//--------

MR::Shader::Shader(ResourceManager* manager, const std::string& name, const std::string& source) : Resource(manager, name, source) {
    //this->Link(sub_shaders, num);
}

MR::Shader::~Shader() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
    if(_res_free_state) {
        for(std::vector<ShaderUniform*>::iterator it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
            delete (*it);
        }
        _shaderUniforms.clear();
    }
}

//SHADER MANAGER

MR::Resource* MR::ShaderManager::Create(const std::string& name, const std::string& source) {
    if(this->_debugMessages) MR::Log::LogString("ShaderManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Shader * s = new Shader(this, name, source);
    this->_resources.push_back(s);
    return s;
}
