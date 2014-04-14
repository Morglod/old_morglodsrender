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
    "layout (location = 0) out vec3 VertexPos;"
    "layout (location = 1) out vec3 VertexNormal;"
    "layout (location = 2) out vec4 VertexColor;"
    "layout (location = 3) out vec2 VertexTexCoord;"
    "void main() {"
    "	VertexPos = position;"
    "	VertexColor = color;"
    "	VertexNormal = normal;"
    "	VertexTexCoord = texCoord;"
    "	gl_Position = (mvp) * vec4(position,1);"
    "}";

const char* code_default_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform vec4 ENGINE_COLOR;"
    "void main() {"
    "	gl_FragColor = texture(ENGINE_ALBEDO, VertexTexCoord) * ENGINE_COLOR;"
    "}";

const char* code_rtt_vert = code_default_vert;

const char* code_rtt_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
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
    "   vec3 a = texture(ENGINE_ENVIRONMENT, normalize(VertexNormal.xyz)).xyz;"
    "   return vec3(1,1,1);"
    "}"
    "void main() {"
    "	fragColor = (texture(ENGINE_ALBEDO, VertexTexCoord).xyz * ENGINE_COLOR.xyz) * ENGINE_LIGHT();"
    "}";

const char* code_rtt_discard_vert = code_rtt_vert;

const char* code_rtt_discard_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal;"
    "layout (location = 2) in vec4 VertexColor;"
    "layout (location = 3) in vec2 VertexTexCoord;"
    "layout (location = 0) out vec3 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "uniform sampler2D screenColor;"
    "uniform sampler2D screenDepth;"
    "uniform vec4 ENGINE_COLOR;"
    "uniform float ENGINE_ALPHA_DISCARD;"
    "void main() {"
    "    vec4 c = texture(ENGINE_ALBEDO, VertexTexCoord);"
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
    "layout (location = 0) out vec3 VertexPos;"
    "layout (location = 1) out vec3 VertexNormal;"
    "layout (location = 2) out vec4 VertexColor;"
    "layout (location = 3) out vec2 VertexTexCoord;"
    "void main() {"
    "	VertexPos = position;"
    "	VertexColor = color;"
    "	VertexNormal = normal;"
    "	VertexTexCoord = texCoord;"
    "	gl_Position = vec4(position,1);"
    "}";

const char* code_screen_frag =
    "#version 330\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#pragma optimize(on)\n"
    "layout (location = 0) in vec3 VertexPos;"
    "layout (location = 1) in vec4 VertexNormal; "
    "layout (location = 2) in vec4 VertexColor; "
    "layout (location = 3) in vec2 VertexTexCoord;"
    "layout (location = 0) out vec3 fragColor;"
    "uniform sampler2D ENGINE_ALBEDO;"
    "void main() {"
    "	fragColor = texture(ENGINE_ALBEDO, VertexTexCoord).xyz;"
    "}";

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
    if(this->_shader != 0) glDeleteObjectARB(this->_shader);
    this->_type = shader_type;
    this->_shader = glCreateShaderObjectARB((unsigned int)shader_type);
    const char* ccode = code.c_str();
    glShaderSourceARB(this->_shader, 1, &ccode, NULL);
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
    return true;
}

std::string MR::SubShader::ToString() {
    return "SubShader";
}

MR::SubShader::SubShader(const std::string& code, const ISubShader::Type& type) : Object(), _shader(0) {
    this->Compile(code, type);
}

MR::SubShader::~SubShader() {
    glDeleteObjectARB(this->_shader);
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
    if(this->_program != 0) glDeleteObjectARB(this->_program);
    this->_program = glCreateProgramObjectARB();
    for(unsigned int i = 0; i < num; ++i) {
        glAttachObjectARB(_program, sub_shaders[i]->GetGPUId());
    }
    glLinkProgramARB(_program);

    int bufflen = 0;
    glGetProgramiv(this->_program, GL_INFO_LOG_LENGTH, &bufflen);
    if(bufflen > 1) {
        GLchar* logString = new GLchar[bufflen + 1];
        glGetProgramInfoLog(this->_program, bufflen, 0, logString);
        MR::Log::LogString("Shader output: "+std::string(logString), MR_LOG_LEVEL_WARNING);

        delete [] logString;
        logString = 0;
    }
    return true;
}

bool MR::Shader::Link() {
    Link(_sub_shaders.data(), _sub_shaders.size());
    return true;
}

std::string MR::Shader::ToString() {
    return "Shader";
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

        Link((ISubShader**)subs, sub_shaders_num);
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

bool MR::Shader::Use(MR::RenderContext* context) {
    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glUseProgramObjectARB(_program);
        for(auto it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
            if((*it)->_value == nullptr) continue;
            if((*it)->_type == ShaderUniform::Types::Int) glProgramUniform1iEXT(_program, (*it)->GetGPULocation(), ((int*)(*it)->_value)[0]);
            if((*it)->_type == ShaderUniform::Types::Float) glProgramUniform1fEXT(_program, (*it)->GetGPULocation(), ((float*)(*it)->_value)[0]);
            if((*it)->_type == ShaderUniform::Types::Vec2) glProgramUniform2fEXT(_program, (*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1]);
            if((*it)->_type == ShaderUniform::Types::Vec3) glProgramUniform3fEXT(_program, (*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1], ((float*)(*it)->_value)[2]);
            if((*it)->_type == ShaderUniform::Types::Vec4) glProgramUniform4fEXT(_program, (*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1], ((float*)(*it)->_value)[2], ((float*)(*it)->_value)[3]);
            if((*it)->_type == ShaderUniform::Types::Mat4) {
                glProgramUniformMatrix4fvEXT(_program, (*it)->GetGPULocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(*it)->GetValue())[0][0]));
            }
        }
    } else {
        glUseProgramObjectARB(_program);
        for(auto it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it) {
            if((*it)->_value == nullptr) continue;
            if((*it)->_type == ShaderUniform::Types::Int) glUniform1i((*it)->GetGPULocation(), ((int*)(*it)->_value)[0]);
            if((*it)->_type == ShaderUniform::Types::Float) glUniform1f((*it)->GetGPULocation(), ((float*)(*it)->_value)[0]);
            if((*it)->_type == ShaderUniform::Types::Vec2) glUniform2f((*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1]);
            if((*it)->_type == ShaderUniform::Types::Vec3) glUniform3f((*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1], ((float*)(*it)->_value)[2]);
            if((*it)->_type == ShaderUniform::Types::Vec4) glUniform4f((*it)->GetGPULocation(), ((float*)(*it)->_value)[0], ((float*)(*it)->_value)[1], ((float*)(*it)->_value)[2], ((float*)(*it)->_value)[3]);
            if((*it)->_type == ShaderUniform::Types::Mat4) {
                glUniformMatrix4fv((*it)->GetGPULocation(), 1, GL_FALSE, (float*)&(((glm::mat4*)(*it)->_value)[0][0]));
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
    for(std::vector<IShaderUniform*>::iterator it = _shaderUniforms.begin(); it != _shaderUniforms.end(); ++it){
        if((*it)->GetName() == uniform_name) return (*it);
    }
    return nullptr;
}

MR::IShaderUniformBlock* MR::Shader::FindShaderUniformBlock(const std::string& uniform_name){
    for(std::vector<IShaderUniformBlock*>::iterator it = _shaderUniformBlocks.begin(); it != _shaderUniformBlocks.end(); ++it){
        if((*it)->GetName() == uniform_name) return (*it);
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
