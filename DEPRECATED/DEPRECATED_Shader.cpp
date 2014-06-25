#include "Shader.hpp"
#include "Log.hpp"
#include "MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <iostream>
#include <fstream>

const char* code_default_vert =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "uniform mat4 mvp;"
    "uniform mat4 viewMatrix;"
    "uniform mat4 projMatrix;"
    "layout (location = 0) in vec3 position;"
    "layout (location = 1) in vec3 normal;"
    "layout (location = 2) in vec4 color;"
    "layout (location = 3) in vec2 texCoord;"
    "layout (location = 0) out vec3 OutVertexPos;"
    "layout (location = 1) out vec3 OutVertexNormal;"
    "layout (location = 2) out vec4 OutVertexColor;"
    "layout (location = 3) out vec2 OutVertexTexCoord;"
    "void main() {"
    "	OutVertexPos = position;"
    "	OutVertexColor = color;"
    "	OutVertexNormal = normal;"
    "	OutVertexTexCoord = texCoord;"
    "	gl_Position = (mvp) * vec4(position,1);"
    "}";

const char* code_default_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 InVertexPos;"
    "layout (location = 1) in vec4 InVertexNormal;"
    "layout (location = 2) in vec4 InVertexColor;"
    "layout (location = 3) in vec2 InVertexTexCoord;"
    "out vec4 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	fragColor = texture(ENGINE_ALBEDO, InVertexTexCoord) * ENGINE_COLOR;"
    "}";

const char* code_rtt_vert = code_default_vert;

const char* code_rtt_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 InVertexPos;"
    "layout (location = 1) in vec4 InVertexNormal;"
    "layout (location = 2) in vec4 InVertexColor;"
    "layout (location = 3) in vec2 InVertexTexCoord;"
    "layout (location = 0) out vec3 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform sampler2D screenColor;"
    "uniform sampler2D screenDepth;"
    "uniform samplerCube ENGINE_ENVIRONMENT;"
    "uniform vec4 ENGINE_COLOR;"
    "vec3 v3pow(vec3 v, float f){"
    "   return pow( v, vec3(f, f, f) );"
    "}"
    "vec3 ENGINE_LIGHT(){"
    "   vec3 a = texture(ENGINE_ENVIRONMENT, normalize(InVertexNormal.xyz)).xyz;"
    "   return vec3(1,1,1);"
    "}"
    "void main() {"
    "	fragColor = (texture(ENGINE_ALBEDO, InVertexTexCoord).xyz * ENGINE_COLOR.xyz) * ENGINE_LIGHT();"
    "}";

const char* code_rtt_discard_vert = code_rtt_vert;

const char* code_rtt_discard_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 InVertexPos;"
    "layout (location = 1) in vec4 InVertexNormal;"
    "layout (location = 2) in vec4 InVertexColor;"
    "layout (location = 3) in vec2 InVertexTexCoord;"
    "layout (location = 0) out vec3 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform sampler2D screenColor;"
    "uniform sampler2D screenDepth;"
    "uniform vec4 ENGINE_COLOR;"
    "uniform float ENGINE_ALPHA_DISCARD;"
    "void main() {"
    "   vec4 c = texture(ENGINE_ALBEDO, InVertexTexCoord);"
    "	if(c.w < ENGINE_ALPHA_DISCARD) discard;"
    "	fragColor = c.xyz * ENGINE_COLOR.xyz;"
    "}";

const char* code_screen_vert =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 position;"
    "layout (location = 1) in vec3 normal;"
    "layout (location = 2) in vec4 color;"
    "layout (location = 3) in vec2 texCoord;"
    "layout (location = 0) out vec3 OutVertexPos;"
    "layout (location = 1) out vec3 OutVertexNormal;"
    "layout (location = 2) out vec4 OutVertexColor;"
    "layout (location = 3) out vec2 OutVertexTexCoord;"
    "void main() {"
    "	OutVertexPos = position;"
    "	OutVertexColor = color;"
    "	OutVertexNormal = normal;"
    "	OutVertexTexCoord = texCoord;"
    "	gl_Position = vec4(position,1);"
    "}";

const char* code_screen_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 InVertexPos;"
    "layout (location = 1) in vec4 InVertexNormal; "
    "layout (location = 2) in vec4 InVertexColor; "
    "layout (location = 3) in vec2 InVertexTexCoord;"
    "layout (location = 0) out vec3 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "void main() {"
    "	fragColor = texture(ENGINE_ALBEDO, InVertexTexCoord).xyz;"
    "}";

bool MR::ShaderUniform::Map(IShader* shader) {
    _uniform_location = glGetUniformLocationARB(shader->GetGPUProgramId(), _name.c_str());
    OnMapped(this, shader, _uniform_location);
    return true;
}

std::string MR::ShaderUniform::ToString() {
    return "ShaderUniform(" + std::string(_name) + ") with data at " + std::to_string((unsigned long) _value);
}

MR::ShaderUniform::ShaderUniform(const char* Name, const IShaderUniform::Types& Type, void* Value, IShader* shader_program) : Object(), IShaderUniform(Name, Type, Value, shader_program), _name(Name) {
    Map(shader_program);
}

MR::ShaderUniform::~ShaderUniform() {
}

//UNIFORM BLOCK

std::string MR::ShaderUniformBlock::ToString() {
    return "ShaderUniformBlock(" + std::string(_name) + ") with data at " + std::to_string((unsigned long) _data);
}

bool MR::ShaderUniformBlock::Map(IShader* shader_program) {
    _uniform_block_index = glGetUniformBlockIndex(shader_program->GetGPUProgramId(), _name.c_str());
    glGetActiveUniformBlockiv(shader_program->GetGPUProgramId(), _uniform_block_index, GL_UNIFORM_BLOCK_DATA_SIZE, &_block_size);
    const char * unames = &_uniform_names[0][0];
    glGetUniformIndices(shader_program->GetGPUProgramId(), _num_uniforms, &unames, _uniform_indecies);
    glGetActiveUniformsiv(shader_program->GetGPUProgramId(), _num_uniforms, _uniform_indecies, GL_UNIFORM_OFFSET, _uniform_offsets);
    return true;
}

bool MR::ShaderUniformBlock::BufferData(unsigned char* data, const size_t& size, const size_t& offset){
    if(_data == NULL){
        _data = data;
        _block_size = size;
        glGenBuffers(1, &_ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
        glBufferData(GL_UNIFORM_BUFFER, _block_size, _data+offset, GL_STREAM_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, _uniform_block_index, _ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    } else {
        glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    return true;
}

MR::ShaderUniformBlock::ShaderUniformBlock(const std::string& Name, const int& NumUniforms, std::string* UniformNames, IShader* shader) :
    Object(), _name(Name), _data(NULL), _uniform_block_index(0), _block_size(0), _num_uniforms(NumUniforms), _uniform_names(UniformNames), _uniform_indecies( new unsigned int[NumUniforms] ), _uniform_offsets( new int[NumUniforms] ), /*type(Type),*/ _ubo(0) {
    Map(shader);
}

MR::ShaderUniformBlock::~ShaderUniformBlock() {
    glDeleteBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

//SUB SHADER
bool MR::SubShader::Compile(const std::string& code, const ISubShader::Type& shader_type) {
    MR::MachineInfo::ClearError();

    if(this->_shader != 0) glDeleteShader(this->_shader);
    this->_type = shader_type;
    this->_shader = glCreateShader((unsigned int)shader_type);
    const char* ccode = code.c_str();
    glShaderSource(this->_shader, 1, &ccode, NULL);
    glCompileShader(this->_shader);

    std::string ErrorOutput = "";
    if(MR::MachineInfo::CatchError(ErrorOutput, NULL)){
        MR::Log::LogString("Error in sub shader compilation \""+ErrorOutput+"\"", MR_LOG_LEVEL_ERROR);
    }

    int bufflen = 0;
    glGetShaderiv(this->_shader, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        std::string logString;
        logString.resize((size_t)bufflen + 1);

        glGetShaderInfoLog(this->_shader, bufflen, 0, &logString[0]);
        MR::Log::LogString("Sub shader output: "+logString, MR_LOG_LEVEL_INFO);

        if(logString.find("error") != std::string::npos) {
            MR::Log::LogString("Sub shader compilation failed", MR_LOG_LEVEL_ERROR);
        }
    }

    int compile_status = -1;
    glGetShaderiv(_shader, GL_COMPILE_STATUS, &compile_status);
    if(compile_status == GL_FALSE){
        MR::Log::LogString("Sub shader compilation failed", MR_LOG_LEVEL_ERROR);
        return false;
    }

    OnCompiled(this, code, shader_type);
    return true;
}

std::string MR::SubShader::ToString() {
    return "SubShader";
}

MR::SubShader::SubShader(const std::string& code, const ISubShader::Type& type) : Object(), _shader(0) {
    this->Compile(code, type);
}

MR::SubShader::~SubShader() {
    glDeleteShader(this->_shader);
}

MR::SubShader* MR::SubShader::FromFile(const std::string& file, const SubShader::Type& shader_type) {
    return new MR::SubShader(textFileRead(file.c_str()), shader_type);
}

MR::SubShader* MR::SubShader::DefaultVert() {
    return new SubShader(code_default_vert, SubShader::Type::Vertex);
}

MR::SubShader* MR::SubShader::DefaultRTTVert() {
    return new SubShader(code_rtt_vert, SubShader::Type::Vertex);
}

MR::SubShader* MR::SubShader::DefaultRTTDiscardVert() {
    return new SubShader(code_rtt_discard_vert, SubShader::Type::Vertex);
}

MR::SubShader* MR::SubShader::DefaultScreenVert() {
    return new SubShader(code_screen_vert, SubShader::Type::Vertex);
}

MR::SubShader* MR::SubShader::DefaultFrag() {
    return new SubShader(code_default_frag, SubShader::Type::Fragment);
}

MR::SubShader* MR::SubShader::DefaultRTTFrag() {
    return new SubShader(code_rtt_frag, SubShader::Type::Fragment);
}

MR::SubShader* MR::SubShader::DefaultRTTDiscardFrag() {
    return new SubShader(code_rtt_discard_frag, SubShader::Type::Fragment);
}

MR::SubShader* MR::SubShader::DefaultScreenFrag() {
    return new SubShader(code_screen_frag, SubShader::Type::Fragment);
}

//SHADER
bool MR::Shader::Link(ISubShader** sub_shaders, const unsigned int& num) {
    MR::MachineInfo::ClearError();

    if(this->_program != 0) glDeleteProgram(this->_program);
    this->_program = glCreateProgram();
    for(unsigned int i = 0; i < num; ++i) {
        glAttachShader(_program, sub_shaders[i]->GetGPUId());
    }
    glLinkProgram(_program);

    std::string ErrorOutput = "";
    if(MR::MachineInfo::CatchError(ErrorOutput, NULL)){
        MR::Log::LogString("Error shader linking \""+ErrorOutput+"\"", MR_LOG_LEVEL_ERROR);
    }

    int bufflen = 0;
    glGetProgramiv(this->_program, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        std::string logString;
        logString.resize(bufflen + 1);

        glGetProgramInfoLog(this->_program, bufflen, 0, &logString[0]);
        MR::Log::LogString("Shader output: "+logString, MR_LOG_LEVEL_INFO);

        if(logString.find("error") != std::string::npos) {
            MR::Log::LogString("Shader linking failed", MR_LOG_LEVEL_ERROR);
            return false;
        }
    }

    int link_status = -1;
    glGetProgramiv(_program, GL_LINK_STATUS, &link_status);
    if(link_status == GL_FALSE){
        MR::Log::LogString("Shader linking failed", MR_LOG_LEVEL_ERROR);
        return false;
    }

    return true;
}

bool MR::Shader::Link() {
    Link(_sub_shaders.data(), _sub_shaders.size());
    dynamic_cast<ShaderManager*>(_resource_manager)->BindDefaultShaderInOut(this);
    return true;
}

std::string MR::Shader::ToString() {
    return "Shader";
}

bool MR::Shader::_Loading() {
    //if(_async_loading_handle.NoErrors()) if(!_async_loading_handle.End()) return false;

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

        Link((ISubShader**)subs, sub_shaders_num);
    } else if(this->_resource_manager->GetDebugMessagesState()) {
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(this->_program == 0) {
        MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") loading failed. GL_PROGRAM is null", MR_LOG_LEVEL_ERROR);
        return false;
    }

    return true;
}

void MR::Shader::_UnLoading() {
    //if(_async_unloading_handle.NoErrors()) _async_unloading_handle.End();

    if(_loaded) {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") unloading", MR_LOG_LEVEL_INFO);
        glDeleteProgram(this->_program);
    }
}

bool MR::Shader::Use(MR::IRenderSystem* context) {
    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glUseProgram(_program);
        for(size_t i = 0; i < _shaderUniforms.size(); ++i) {
            if((_shaderUniforms[i])->_value == nullptr) continue;
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Int) glProgramUniform1iEXT(_program, (_shaderUniforms[i])->GetGPULocation(), ((int*)(_shaderUniforms[i])->_value)[0]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Float) glProgramUniform1fEXT(_program, (_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec2) glProgramUniform2fEXT(_program, (_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec3) glProgramUniform3fEXT(_program, (_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1], ((float*)(_shaderUniforms[i])->_value)[2]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec4) glProgramUniform4fEXT(_program, (_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1], ((float*)(_shaderUniforms[i])->_value)[2], ((float*)(_shaderUniforms[i])->_value)[3]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Mat4) {
                glProgramUniformMatrix4fvEXT(_program, (_shaderUniforms[i])->GetGPULocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(_shaderUniforms[i])->GetValue())[0][0]));
            }
        }
    } else {
        glUseProgram(_program);
        for(size_t i = 0; i < _shaderUniforms.size(); ++i) {
            if((_shaderUniforms[i])->_value == nullptr) continue;
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Int) glUniform1i((_shaderUniforms[i])->GetGPULocation(), ((int*)(_shaderUniforms[i])->_value)[0]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Float) glUniform1f((_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec2) glUniform2f((_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec3) glUniform3f((_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1], ((float*)(_shaderUniforms[i])->_value)[2]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Vec4) glUniform4f((_shaderUniforms[i])->GetGPULocation(), ((float*)(_shaderUniforms[i])->_value)[0], ((float*)(_shaderUniforms[i])->_value)[1], ((float*)(_shaderUniforms[i])->_value)[2], ((float*)(_shaderUniforms[i])->_value)[3]);
            if((_shaderUniforms[i])->_type == ShaderUniform::Types::Mat4) {
                glUniformMatrix4fv((_shaderUniforms[i])->GetGPULocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(_shaderUniforms[i])->_value)[0][0]));
            }
        }
    }
    return true;
}

//--------

MR::IShaderUniform* MR::Shader::CreateUniform(const std::string& uniform_name, const MR::ShaderUniform::Types& type, void* value){
    MR::ShaderUniform* p = new MR::ShaderUniform(uniform_name.c_str(), type, value, this);
    _shaderUniforms.push_back(p);
    return p;
}

void MR::Shader::SetUniform(const std::string& name, const MR::IShaderUniform::Types& type, void* value) {
    if(!value) return;
    switch(type) {
    case ShaderUniform::Types::Int:
        glProgramUniform1iEXT(_program, glGetUniformLocation(_program, name.c_str()), ((int*)value)[0]);
        break;
    case ShaderUniform::Types::Float:
        glProgramUniform1fEXT(_program, glGetUniformLocation(_program, name.c_str()), ((float*)value)[0]);
        break;
    case ShaderUniform::Types::Vec2:
        glProgramUniform2fEXT(_program, glGetUniformLocation(_program, name.c_str()), ((float*)value)[0], ((float*)value)[1]);
        break;
    case ShaderUniform::Types::Vec3:
        glProgramUniform3fEXT(_program, glGetUniformLocation(_program, name.c_str()), ((float*)value)[0], ((float*)value)[1], ((float*)value)[2]);
        break;
    case ShaderUniform::Types::Vec4:
        glProgramUniform4fEXT(_program, glGetUniformLocation(_program, name.c_str()), ((float*)value)[0], ((float*)value)[1], ((float*)value)[2], ((float*)value)[3]);
        break;
    case ShaderUniform::Types::Mat4:
        glProgramUniformMatrix4fvEXT(_program, glGetUniformLocation(_program, name.c_str()), 1, GL_FALSE, (float*)&(((glm::mat4*)value)[0][0]));
        break;
    }
}

MR::IShaderUniformBlock* MR::Shader::CreateUniformBlock(const std::string& Name, const int& NumUniforms, std::string* UniformNames) {
    MR::ShaderUniformBlock* p = new MR::ShaderUniformBlock(Name, NumUniforms,UniformNames,this);
    _shaderUniformBlocks.push_back(p);
    return p;
}

void MR::Shader::DeleteUniform(MR::IShaderUniform* su){
    std::vector<IShaderUniform*>::iterator it = std::find(_shaderUniforms.begin(), _shaderUniforms.end(), su);
    if(it == _shaderUniforms.end()) return;
    delete (*it);
    _shaderUniforms.erase(it);
}

void MR::Shader::DeleteUniformBlock(MR::IShaderUniformBlock* sub){
    std::vector<IShaderUniformBlock*>::iterator it = std::find(_shaderUniformBlocks.begin(), _shaderUniformBlocks.end(), sub);
    if(it == _shaderUniformBlocks.end()) return;
    delete (*it);
    _shaderUniformBlocks.erase(it);
}

MR::IShaderUniform* MR::Shader::FindShaderUniform(const std::string& uniform_name){
    for(size_t i = 0; i < _shaderUniforms.size(); ++i){
        if((_shaderUniforms[i])->GetName() == uniform_name) return (_shaderUniforms[i]);
    }
    return nullptr;
}

MR::IShaderUniformBlock* MR::Shader::FindShaderUniformBlock(const std::string& uniform_name){
    for(size_t i = 0; i < _shaderUniformBlocks.size(); ++i){
        if((_shaderUniformBlocks[i])->GetName() == uniform_name) return (_shaderUniformBlocks[i]);
    }
    return nullptr;
}

void MR::Shader::AttachSubShader(ISubShader* sub){
    _sub_shaders.push_back(sub);
}

void MR::Shader::DetachSubShader(ISubShader* sub){
    std::vector<ISubShader*>::iterator it = std::find(_sub_shaders.begin(), _sub_shaders.end(), sub);
    if(it == _sub_shaders.end()) return;
    _sub_shaders.erase(it);
}

void MR::Shader::DetachAllSubShaders(){
    _sub_shaders.clear();
}

MR::IShader::ShaderFeatures MR::Shader::GetFeatures(){
    return _features;
}

MR::Shader::Shader(ResourceManager* manager, const std::string& name, const std::string& source) : Resource(manager, name, source), Object(), _program(0) {
    //this->Link(sub_shaders, num);
}

MR::Shader::~Shader() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Shader "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
    if(_res_free_state) {
        for(std::vector<IShaderUniform*>::iterator it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
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

MR::ShaderManager* __INSTANCE_SHADER_MANAGER = 0;
MR::ShaderManager* MR::ShaderManager::Instance() {
    if(__INSTANCE_SHADER_MANAGER == 0) __INSTANCE_SHADER_MANAGER = new MR::ShaderManager();
    return __INSTANCE_SHADER_MANAGER;
}

void MR::ShaderManager::DestroyInstance() {
    delete __INSTANCE_SHADER_MANAGER;
    __INSTANCE_SHADER_MANAGER = 0;
}

bool MR::ShaderManager::BindDefaultShaderInOut(IShader* shader){
    if(!shader) return false;

    MR::MachineInfo::ClearError();

    bool status = true;
    std::string errorStr = "";
    unsigned int p = shader->GetGPUProgramId();
    if(glIsProgram(p)){
        glBindAttribLocation(p, MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION, MR_SHADER_VERTEX_POSITION_ATTRIB_NAME);

        if(MR::MachineInfo::CatchError(errorStr, NULL)){
            MR::Log::LogString("Error while binding vertex position shader's attribute \""+errorStr+"\"", MR_LOG_LEVEL_ERROR);
            status = false;
        }

        glBindAttribLocation(p, MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION, MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME);

        if(MR::MachineInfo::CatchError(errorStr, NULL)){
            MR::Log::LogString("Error while binding vertex normal shader's attribute \""+errorStr+"\"", MR_LOG_LEVEL_ERROR);
            status = false;
        }

        glBindAttribLocation(p, MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION, MR_SHADER_VERTEX_COLOR_ATTRIB_NAME);

        if(MR::MachineInfo::CatchError(errorStr, NULL)){
            MR::Log::LogString("Error while binding vertex color shader's attribute \""+errorStr+"\"", MR_LOG_LEVEL_ERROR);
            status = false;
        }

        glBindAttribLocation(p, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION, MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME);

        if(MR::MachineInfo::CatchError(errorStr, NULL)){
            MR::Log::LogString("Error while binding vertex texcoord shader's attribute \""+errorStr+"\"", MR_LOG_LEVEL_ERROR);
            status = false;
        }

        glBindFragDataLocation(p, MR_SHADER_DEFAULT_FRAG_DATA_LOCATION, MR_SHADER_DEFAULT_FRAG_DATA_NAME);

        if(MR::MachineInfo::CatchError(errorStr, NULL)){
            MR::Log::LogString("Error while binding shader's frag data output \""+errorStr+"\"", MR_LOG_LEVEL_ERROR);
            status = false;
        }

        glBindFragDataLocation(p, MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_LOCATION, MR_SHADER_DEFAULT_DEPTH_TEX_COORD_DATA_NAME);
        glBindFragDataLocation(p, MR_SHADER_DEFAULT_NORMAL_DATA_LOCATION, MR_SHADER_DEFAULT_NORMAL_DATA_NAME);
        glBindFragDataLocation(p, MR_SHADER_DEFAULT_POS_DATA_LOCATION, MR_SHADER_DEFAULT_POS_DATA_NAME);
    }
    return status;
}