#include "ShaderObjects.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Debug.hpp"
#include "../Utils/Log.hpp"
#include "ShaderConfig.hpp"
#include "ShaderUniforms.hpp"
#include "ShaderCompiler.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

mr::IShaderProgram* __MR_USED_SHADER_PROGRAM = nullptr;
mr::TDynamicArray<mr::IShaderProgram*> _MR_REGISTERED_SHADER_PROGRAMS_;

namespace mr {

/** SHADER CLASS **/

bool Shader::Compile(IShader::Type const& type, std::string const& code) {
    _compiled = false;

    if(_handle == 0) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        int gl_er = 0;
        mr::MachineInfo::ClearError();
#endif
         _handle = glCreateShader(type);
         OnGPUHandleChanged(dynamic_cast<IGPUObjectHandle*>(this), _handle);
#ifdef MR_CHECK_LARGE_GL_ERRORS
        if(mr::MachineInfo::CatchError(0, &gl_er)) {
            std::string err_str = "Error in Shader::Compile : glCreateShader ended with \"" + std::to_string(gl_er) + "\" code. ";
            switch(gl_er) {
            case GL_INVALID_ENUM:
                err_str += "shaderType is not an accepted value."; break;
            default:
                err_str += "Unknow code."; break;
            }
            mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
            return false;
        }
#endif
    }

    mr::ShaderCompiler compiler;
    compiler.debug_log = false;
    mr::ShaderCompilationOutput out = compiler.Compile(code, (IShaderCompiler::ShaderType)type, _handle);
    compiler.Release();

    if(!out.Good()) {
        mr::Log::LogString("- Failed Shader::Compile -", MR_LOG_LEVEL_ERROR);
        for(size_t i = 0; i < out.GetMessagesNum(); ++i) {
            mr::ShaderCompilationMessage& msg = out.GetMessage(i);
            mr::Log::LogString(ShaderCompilationMessage::MessageTypeNames[msg.GetType()] + " [" + std::to_string(msg.GetCode()) + "] in line " + std::to_string(msg.GetLine()) + " \"" + msg.GetText() + "\" ", MR_LOG_LEVEL_WARNING);
        }
        mr::Log::LogString("- - -", MR_LOG_LEVEL_ERROR);
        return false;
    }

    _type = type;
    _compiled = true;
    OnCompiled(dynamic_cast<mr::IShader*>(this), code, type);
    return true;
}

void Shader::Destroy() {
    _compiled = false;
    if(_handle != 0) {
        glDeleteShader(_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<IGPUObjectHandle*>(this), _handle);
        OnDestroy(dynamic_cast<IObjectHandle*>(this));
    }
}

Shader::Shader() : _type(None), _compiled(false) {
}

Shader::~Shader() {
}

Shader* Shader::CreateAndCompile(const IShader::Type& type, const std::string& code) {
    Shader* sh = new mr::Shader();
    if(!sh->Compile(type, code)) {
        sh->Destroy();
        delete sh;
        return nullptr;
    }
    return sh;
}

/** SHADER PROGRAM **/

bool ShaderProgram::Link(mu::ArrayHandle<IShader*> shaders) {
    _linked = false;
    Assert(shaders.GetNum() == 0)

    if(_handle == 0) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
        std::string gl_str = "";
        mr::MachineInfo::ClearError();
#endif
        _handle = glCreateProgram();
        OnGPUHandleChanged(dynamic_cast<IGPUObjectHandle*>(this), _handle);
#ifdef MR_CHECK_LARGE_GL_ERRORS
        if(mr::MachineInfo::CatchError(&gl_str, 0)) {
            mr::Log::LogString("Failed ShaderProgram::Link(). Failed creating OpenGL shader program. " + gl_str, MR_LOG_LEVEL_ERROR);
            return false;
        }
#endif
    }

    mr::TStaticArray<unsigned int> shadersHandles(new unsigned int [shaders.GetNum()], shaders.GetNum(), true);
    unsigned int* handlesPtr = shadersHandles.GetRaw();
    for(size_t i = 0; i < shaders.GetNum(); ++i){
        handlesPtr[i] = shaders.GetArray()[i]->GetGPUHandle();
    }

    mr::ShaderCompiler compiler;
    compiler.debug_log = false;
    mr::ShaderCompilationOutput out = compiler.Link(shadersHandles, _handle);
    compiler.Release();

    if(!out.Good()) {
        mr::Log::LogString("- Failed ShaderProgram::Link -", MR_LOG_LEVEL_ERROR);
        for(size_t i = 0; i < out.GetMessagesNum(); ++i) {
            mr::ShaderCompilationMessage& msg = out.GetMessage(i);
            mr::Log::LogString(ShaderCompilationMessage::MessageTypeNames[msg.GetType()] + " [" + std::to_string(msg.GetCode()) + "] in line " + std::to_string(msg.GetLine()) + " \"" + msg.GetText() + "\" ", MR_LOG_LEVEL_WARNING);
        }
        mr::Log::LogString("- - -", MR_LOG_LEVEL_ERROR);
        return false;
    }
    _linked = true;
    return true;
}

IShaderUniform* ShaderProgram::CreateUniform(const std::string& name, const mr::IShaderUniform::Type& type, void* value) {
    ShaderUniform* su = new ShaderUniform(name, type, value, dynamic_cast<IShaderProgram*>(this));
    _shaderUniforms.push_back(su);
    return su;
}

void ShaderProgram::SetUniform(const std::string& name, const int& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform1i(_handle, glGetUniformLocation(_handle, name.c_str()), value);
        else glUniform1i(glGetUniformLocation(_handle, name.c_str()), value);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const float& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform1f(_handle, glGetUniformLocation(_handle, name.c_str()), value);
        else glUniform1f(glGetUniformLocation(_handle, name.c_str()), value);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec2& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform2fv(_handle, glGetUniformLocation(_handle, name.c_str()), 2, &value.x);
        else glUniform2fv(glGetUniformLocation(_handle, name.c_str()), 2, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec3& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform3fv(_handle, glGetUniformLocation(_handle, name.c_str()), 3, &value.x);
        else glUniform3fv(glGetUniformLocation(_handle, name.c_str()), 3, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::vec4& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniform4fv(_handle, glGetUniformLocation(_handle, name.c_str()), 4, &value.x);
        else glUniform4fv(glGetUniformLocation(_handle, name.c_str()), 4, &value.x);
    }
}

void ShaderProgram::SetUniform(const std::string& name, const glm::mat4& value) {
    if(_handle) {
        if(mr::gl::IsDirectStateAccessSupported()) glProgramUniformMatrix4fv(_handle, glGetUniformLocation(_handle, name.c_str()), 1,  GL_FALSE, &value[0][0]);
        else glUniformMatrix4fv(glGetUniformLocation(_handle, name.c_str()), 1,  GL_FALSE, &value[0][0]);
    }
}

void ShaderProgram::DeleteUniform(IShaderUniform* su) {
    auto it = std::find(_shaderUniforms.begin(), _shaderUniforms.end(), su);
    if(it == _shaderUniforms.end()) return;
    delete (*it);
    _shaderUniforms.erase(it);
}

IShaderUniform* ShaderProgram::FindShaderUniform(const std::string& name) {
    for(size_t i = 0; i < _shaderUniforms.size(); ++i){
        if(_shaderUniforms[i]->GetName() == name) return _shaderUniforms[i];
    }
    return nullptr;
}

mu::ArrayHandle<IShaderUniform*> ShaderProgram::GetShaderUniforms() {
    return mu::ArrayHandle<IShaderUniform*>(&_shaderUniforms[0], _shaderUniforms.size(), false);
}

mu::ArrayHandle<ShaderUniformInfo> ShaderProgram::GetCompiledUniforms() {
    int act_uniforms = 0;
    glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &act_uniforms);
    if(act_uniforms == 0) return mu::ArrayHandle<ShaderUniformInfo>();

    ShaderUniformInfo* uni = new ShaderUniformInfo[act_uniforms];

    for(int iu = 0; iu < act_uniforms; ++iu){
        char namebuffer[1024];
        int real_buf_size = 0;
        int unif_size = 0;
        unsigned int uni_type = 0;
        glGetActiveUniform(_handle, iu, 1024, &real_buf_size, &unif_size, &uni_type, &namebuffer[0]);
        uni[iu] = ShaderUniformInfo(dynamic_cast<IShaderProgram*>(this), std::string(namebuffer), unif_size, uni_type);
    }

    return mu::ArrayHandle<ShaderUniformInfo>(&uni[0], act_uniforms, false);
}

bool ShaderProgram::IsUniform(std::string const& uniformName) {
    for(size_t i = 0; i < _shaderUniforms.size(); ++i){
        if(_shaderUniforms[i]->GetName() == uniformName) return true;
    }
    return false;
}

void ShaderProgram::UpdateUniforms() {
    IShaderUniform** su = &_shaderUniforms[0];
    for(size_t i = 0; i < _shaderUniforms.size(); ++i){
        su[i]->Update();
    }
}

ShaderProgramCache ShaderProgram::GetCache() {
    if(_handle == 0) return ShaderProgramCache();

    int bin_length = 0, _a = 0;
    unsigned int form = 0;
    glGetProgramiv(_handle, GL_PROGRAM_BINARY_LENGTH, &bin_length);
    unsigned char * buf = new unsigned char[bin_length];

    glGetProgramBinary(_handle, bin_length, &_a, &form, &buf[0]);
    return ShaderProgramCache(form, mu::ArrayHandle<unsigned char>(&buf[0], bin_length, true));
}

void ShaderProgram::Destroy() {
    if(_handle != 0) {
        glDeleteProgram(_handle);
        _handle = 0;

        OnGPUHandleChanged(dynamic_cast<IGPUObjectHandle*>(this), _handle);
        OnDestroy(dynamic_cast<IObjectHandle*>(this));
    }
}

bool ShaderProgram::Use() {
    if(_handle == 0) {
#ifdef MR_CHECK_SMALL_GL_ERRORS
        mr::Log::LogString("Failed ShaderProgram::Use(). Can't use not created shader program.", MR_LOG_LEVEL_ERROR);
#endif
        return false;
    }
    if(!_linked) {
#ifdef MR_CHECK_SMALL_GL_ERRORS
        mr::Log::LogString("Failed ShaderProgram::Use(). Can't use not linked shader program.", MR_LOG_LEVEL_ERROR);
#endif
        return false;
    }
    IShaderProgram* ptr = dynamic_cast<IShaderProgram*>(this);
    if(__MR_USED_SHADER_PROGRAM != ptr) {
        glUseProgram(_handle);
        __MR_USED_SHADER_PROGRAM = ptr;
    }
    UpdateUniforms();
    return true;
}

ShaderProgram::ShaderProgram() : _shaderUniforms(), _linked(false) {
    _MR_REGISTERED_SHADER_PROGRAMS_.PushBack(dynamic_cast<mr::IShaderProgram*>(this));
}

ShaderProgram::~ShaderProgram() {
    _MR_REGISTERED_SHADER_PROGRAMS_.Erase(dynamic_cast<mr::IShaderProgram*>(this));
}

ShaderProgram* ShaderProgram::CreateAndLink(mu::ArrayHandle<IShader*> shaders) {
    ShaderProgram* sp = new mr::ShaderProgram();
    if(!sp->Link(shaders)) {
        sp->Destroy();
        delete sp;
        return nullptr;
    }
    return sp;
}

ShaderProgram* ShaderProgram::Default() {
    std::string vs =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "layout (location = "+std::to_string(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION)+") in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION)+") in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "out vec4 MR_VertexPos;\n"
        "out vec4 MR_LocalVertexPos;\n"
        "out vec3 MR_VertexNormal;\n"
        "out vec4 MR_VertexColor;\n"
        "out vec2 MR_VertexTexCoord;\n"

        "void main() {\n"
        "	vec4 pos = "+std::string(MR_SHADER_MVP_MAT4)+" * "+std::string(MR_SHADER_MODEL_MAT4)+" * vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
        "	MR_VertexPos = pos;\n"
        "   MR_LocalVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
        "   MR_VertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "	MR_VertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "	MR_VertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
        "	gl_Position = pos;\n"
        "}";
    std::string fs =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "in vec4 MR_VertexPos;\n"
        "in vec4 MR_LocalVertexPos;\n"
        "smooth in vec3 MR_VertexNormal;\n"
        "in vec4 MR_VertexColor;\n"
        "in vec2 MR_VertexTexCoord;\n"
        "\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_POS)+";\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_DIR)+";\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "uniform sampler2D "+std::string(MR_SHADER_COLOR_TEX)+";\n"
        "uniform vec4 "+std::string(MR_SHADER_COLOR_V)+";\n"

        "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+";\n"

        "void main() {"
        "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = (texture("+std::string(MR_SHADER_COLOR_TEX)+", MR_VertexTexCoord) * "+std::string(MR_SHADER_COLOR_V)+");\n"
        "}";

    IShader* sh[2] { dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Vertex, vs)), dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Fragment, fs))};
    if(sh[0] == nullptr || sh[1] == nullptr) return nullptr;
    ShaderProgram* sp = ShaderProgram::CreateAndLink(mu::ArrayHandle<IShader*>(&sh[0], 2, false));
    sh[0]->Destroy(); sh[1]->Destroy();
    delete sh[0]; delete sh[1];
    return sp;
}

ShaderProgram* ShaderProgram::DefaultWithTexture() {
    std::string vs =
        "#version 330 \n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "layout (location = "+std::to_string(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION)+") in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION)+") in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
        //TODO
        "layout (location = 4) in vec3 _TEST_INST;\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "out vec4 MR_VertexPos;\n"
        "out vec4 MR_LocalVertexPos;\n"
        "smooth out vec3 MR_VertexNormal;\n"
        "out vec4 MR_VertexColor;\n"
        "out vec2 MR_VertexTexCoord;\n"

        "void main() {\n"
        //TODO
        "	vec4 pos = "+std::string(MR_SHADER_MVP_MAT4)+" * "+std::string(MR_SHADER_MODEL_MAT4)+" * vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+" + _TEST_INST, 1);\n"
        "	MR_VertexPos = pos;\n"
        "   MR_LocalVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
        "   MR_VertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "	MR_VertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "	MR_VertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
        "	gl_Position = pos;\n"
        "}";
    std::string fs =
        "#version 330 \n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "in vec4 MR_VertexPos;\n"
        "in vec4 MR_LocalVertexPos;\n"
        "smooth in vec3 MR_VertexNormal;\n"
        "in vec4 MR_VertexColor;\n"
        "in vec2 MR_VertexTexCoord;\n"
        "\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_POS)+";\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_DIR)+";\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "uniform sampler2D "+std::string(MR_SHADER_COLOR_TEX)+";\n"
        "uniform vec4 "+std::string(MR_SHADER_COLOR_V)+";\n"

        "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+";\n"

        "void main() {"
        "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = texture("+std::string(MR_SHADER_COLOR_TEX)+", MR_VertexTexCoord) * vec4(MR_VertexNormal, 1);"
        "}";

    IShader* sh[2] { dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Vertex, vs)), dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Fragment, fs))};
    if(sh[0] == nullptr || sh[1] == nullptr) return nullptr;
    ShaderProgram* sp = ShaderProgram::CreateAndLink(mu::ArrayHandle<IShader*>(&sh[0], 2, false));
    sh[0]->Destroy(); sh[1]->Destroy();
    delete sh[0]; delete sh[1];
    return sp;
}

ShaderProgram* ShaderProgram::DefaultBase() {
    std::string vs =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "layout (location = "+std::to_string(MR_SHADER_VERTEX_POSITION_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_NORMAL_ATTRIB_LOCATION)+") in vec3 "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_COLOR_ATTRIB_LOCATION)+") in vec4 "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "layout (location = "+std::to_string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_LOCATION)+") in vec2 "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "out vec4 MR_VertexPos;\n"
        "out vec4 MR_LocalVertexPos;\n"
        "smooth out vec3 MR_VertexNormal;\n"
        "out vec4 MR_VertexColor;\n"
        "out vec2 MR_VertexTexCoord;\n"

        "void main() {\n"
        "	vec4 pos = "+std::string(MR_SHADER_MVP_MAT4)+" * "+std::string(MR_SHADER_MODEL_MAT4)+"* vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
        "	MR_VertexPos = pos;\n"
        "   MR_LocalVertexPos = vec4("+std::string(MR_SHADER_VERTEX_POSITION_ATTRIB_NAME)+",1);\n"
        "   MR_VertexNormal = "+std::string(MR_SHADER_VERTEX_NORMAL_ATTRIB_NAME)+";\n"
        "	MR_VertexColor = "+std::string(MR_SHADER_VERTEX_COLOR_ATTRIB_NAME)+";\n"
        "	MR_VertexTexCoord = "+std::string(MR_SHADER_VERTEX_TEXCOORD_ATTRIB_NAME)+";\n"
        "	gl_Position = pos;\n"
        "}";
    std::string fs =
        "#version 150 core\n"
        "#extension GL_ARB_separate_shader_objects : enable\n"
        "#pragma optimize (on)\n"
        "#pragma optionNV(fastmath on)\n"
        "#pragma optionNV(fastprecision on)\n"
        "#pragma optionNV(ifcvt none)\n"
        "#pragma optionNV(inline all)\n"
        "#pragma optionNV(strict on)\n"
        "#pragma optionNV(unroll all)\n"
        "precision mediump float;\n"

        "in vec4 MR_VertexPos;\n"
        "in vec4 MR_LocalVertexPos;\n"
        "smooth in vec3 MR_VertexNormal;\n"
        "in vec4 MR_VertexColor;\n"
        "in vec2 MR_VertexTexCoord;\n"
        "\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_POS)+";\n"
        "uniform vec3 "+std::string(MR_SHADER_CAM_DIR)+";\n"

        "uniform mat4 "+std::string(MR_SHADER_MVP_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_MODEL_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_VIEW_MAT4)+";\n"
        "uniform mat4 "+std::string(MR_SHADER_PROJ_MAT4)+";\n"

        "uniform sampler2D "+std::string(MR_SHADER_COLOR_TEX)+";\n"
        "uniform vec4 "+std::string(MR_SHADER_COLOR_V)+";\n"

        "out vec4 "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+";\n"

        "void main() {"
        "   "+std::string(MR_SHADER_DEFAULT_FRAG_DATA_NAME_1)+" = texture("+std::string(MR_SHADER_COLOR_TEX)+", MR_VertexTexCoord);"
        "}";

    IShader* sh[2] { dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Vertex, vs)), dynamic_cast<mr::IShader*>(Shader::CreateAndCompile(IShader::Type::Fragment, fs))};
    if(sh[0] == nullptr || sh[1] == nullptr) return nullptr;
    ShaderProgram* sp = ShaderProgram::CreateAndLink(mu::ArrayHandle<IShader*>(&sh[0], 2, false));
    sh[0]->Destroy(); sh[1]->Destroy();
    delete sh[0]; delete sh[1];
    return sp;
}

ShaderProgram* ShaderProgram::FromCache(ShaderProgramCache cache) {
    if(cache.format == 0) {
        mr::Log::LogString("Failed ShaderProgram::FromCache. Invalid cache format.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    if(cache.data.GetNum() == 0) {
        mr::Log::LogString("Failed ShaderProgram::FromCache. Invalid cache data.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    unsigned int sp_handle = glCreateProgram();
    if(sp_handle == 0) {
        mr::Log::LogString("Failed ShaderProgram::FromCache. Failed creating OpenGL shader program.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

#ifdef MR_CHECK_LARGE_GL_ERRORS
    std::string gl_str = "";
    mr::MachineInfo::ClearError();
#endif
     glProgramBinary(sp_handle, cache.format, cache.data.GetArray(), cache.data.GetNum());
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(&gl_str, 0)) {
        mr::Log::LogString("Failed ShaderProgram::FromCache. Failed loading shader program binary. " + gl_str, MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
#endif

    int link_status = -1;
    glGetProgramiv(sp_handle, GL_LINK_STATUS, &link_status);
    if(link_status != GL_TRUE){
        mr::Log::LogString("Failed ShaderProgram::FromCache. Shader program linking failed.", MR_LOG_LEVEL_ERROR);
        glDeleteProgram(sp_handle);
        return nullptr;
    }

    ShaderProgram* prog = new ShaderProgram();
    prog->_linked = true;
    prog->_handle = sp_handle;
    return prog;
}

/** **/

void UseNullShaderProgram() {
    __MR_USED_SHADER_PROGRAM = nullptr;
    glUseProgram(0);
}

void DestroyAllShaderPrograms() {
    for(size_t i = 0; i < _MR_REGISTERED_SHADER_PROGRAMS_.GetNum(); ++i) {
        _MR_REGISTERED_SHADER_PROGRAMS_.At(i)->Destroy();
    }
}

IShaderProgram* GetUsedShaderProgram() {
    return __MR_USED_SHADER_PROGRAM;
}

}
