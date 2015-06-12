#include "ShaderManager.hpp"
#include "ShaderObjects.hpp"
#include "ShaderProgramObject.hpp"
#include "ShaderCompiler.hpp"
#include "ShaderCodeDefault.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"

#include "ShaderConfig.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

template<>
mr::ShaderManager* mu::Singleton<mr::ShaderManager>::_singleton_instance = nullptr;

namespace mr {

void ShaderManager::SetGlobalUniform(ShaderUniformDesc const& desc, const void* value, bool oneTime) {
    if(oneTime) {
        for(IShaderProgram* prog : _programs) {
            ShaderUniformMap* map = prog->GetMap();
            map->SetUniform(map->GetUniformGPULocation(desc.name), desc.type, value);
        }
        return;
    }

    _globalUniforms[desc.name] = value;
    _globalUniformsDesc[desc.name] = desc;

    UpdateGlobalUniform(desc.name, value);
}

void ShaderManager::DeleteGlobalUniform(ShaderUniformDesc const& desc) {
    _globalUniforms.erase(desc.name);
    _globalUniformsDesc.erase(desc.name);
}

void ShaderManager::SetUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer) {
    _globalUbos[index] = buffer;
}

void ShaderManager::RemoveUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer) {
    _globalUbos.erase(index);
}

void ShaderManager::UpdateGlobalUniform(std::string const& name) {
    const void* value = _globalUniforms[name];
    IShaderUniformRef::Type type = _globalUniformsDesc[name].type;
    for(IShaderProgram* prog : _programs) {
        ShaderUniformMap* map = prog->GetMap();
        map->SetUniform(map->GetUniformGPULocation(name), type, value);
    }
}

void ShaderManager::UpdateGlobalUniform(std::string const& name, const void* value) {
    _globalUniforms[name] = value;
    UpdateGlobalUniform(name);
}

void ShaderManager::UpdateAllGlobalUniforms() {
    for(IShaderProgram* prog : _programs) {
        ShaderUniformMap* map = prog->GetMap();
        for(std::pair<const std::string, const void*>& u : _globalUniforms) {
            map->SetUniform(map->GetUniformGPULocation(u.first), _globalUniformsDesc[u.first].type, u.second);
        }
    }
}

void ShaderManager::SetNVVBUMPointer(int const& vertexAttributeLocation, uint64_t const& residentPointer) {
    for(IShaderProgram* prog : _programs) {
        ShaderUniformMap* map = prog->GetMap();
        map->SetNVVBUMPointer(vertexAttributeLocation, residentPointer);
    }
}

IShader* ShaderManager::CreateShader(IShader::Type const& type) {
    Shader* shader = new Shader();
    if(!shader->Create(type)) {
        mr::Log::LogString("Failed ShaderManager::CreateShader. Shader object not created.", MR_LOG_LEVEL_ERROR);
        shader->Destroy();
        delete shader;
        return nullptr;
    }
    return static_cast<IShader*>(shader);
}

IShader* ShaderManager::CreateAndCompile(IShader::Type const& type, std::string const& code) {
    IShader* ishader = CreateShader(type);
    if(!ishader) {
        mr::Log::LogString("Failed ShaderManager::CreateAndCompile. Shader object not created.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    Shader* shader = static_cast<Shader*>(ishader);
    shader->_compiled = false;

    mr::ShaderCompilationOutput out = _shaderCompiler->Compile(ishader, code);
    if(!out.Good()) {
        mr::Log::LogString("- Failed ShaderManager::CreateAndCompile -", MR_LOG_LEVEL_ERROR);
        for(size_t i = 0; i < out.GetMessagesNum(); ++i) {
            mr::ShaderCompilationMessage& msg = out.GetMessage(i);
            mr::Log::LogString(ShaderCompilationMessage::MessageTypeNames[msg.GetType()] + " [" + std::to_string(msg.GetCode()) + "] in line " + std::to_string(msg.GetLine()) + " \"" + msg.GetText() + "\" ", MR_LOG_LEVEL_WARNING);
        }
        mr::Log::LogString("- - -", MR_LOG_LEVEL_ERROR);
        mr::Log::LogString("Failed ShaderManager::CreateAndCompile. Shader object not compiled.", MR_LOG_LEVEL_ERROR);
        shader->Destroy();
        delete shader;
        return nullptr;
    }

    shader->_compiled = true;
    return ishader;
}

IShaderProgram* ShaderManager::CreateShaderProgram() {
    ShaderProgram* shaderProgram = new ShaderProgram();
    if(!shaderProgram->Create()) {
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgram. Shader program object not created.", MR_LOG_LEVEL_ERROR);
        shaderProgram->Destroy();
        delete shaderProgram;
        return nullptr;
    }

    IShaderProgram* ishaderProgram = static_cast<IShaderProgram*>(shaderProgram);
    RegisterShaderProgram(ishaderProgram);
    return ishaderProgram;
}

IShaderProgram* ShaderManager::CreateAndLink(mu::ArrayHandle<IShader*> shaders) {
    IShaderProgram* ishaderProgram = CreateShaderProgram();
    if(!ishaderProgram) {
        mr::Log::LogString("Failed ShaderManager::CreateAndLink. Shader program object not created.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    ShaderProgram* shaderProgram = static_cast<ShaderProgram*>(ishaderProgram);
    shaderProgram->_linked = false;

    mr::ShaderCompilationOutput out = _shaderCompiler->Link(ishaderProgram, shaders);
    if(!out.Good()) {
        mr::Log::LogString("- Failed ShaderManager::CreateAndLink -", MR_LOG_LEVEL_ERROR);
        for(size_t i = 0; i < out.GetMessagesNum(); ++i) {
            mr::ShaderCompilationMessage& msg = out.GetMessage(i);
            mr::Log::LogString(ShaderCompilationMessage::MessageTypeNames[msg.GetType()] + " [" + std::to_string(msg.GetCode()) + "] in line " + std::to_string(msg.GetLine()) + " \"" + msg.GetText() + "\" ", MR_LOG_LEVEL_WARNING);
        }
        mr::Log::LogString("- - -", MR_LOG_LEVEL_ERROR);
        mr::Log::LogString("Failed ShaderManager::CreateAndLink. Shader program object not linked.", MR_LOG_LEVEL_ERROR);
        shaderProgram->Destroy();
        delete shaderProgram;
        return nullptr;
    }

    shaderProgram->_linked = true;
    shaderProgram->GetMap()->Reset(false);
    return ishaderProgram;
}

ShaderManager::ShaderManager() {
    _shaderCompiler = static_cast<mr::IShaderCompiler*>(mr::ShaderCompiler::GetInstance());
    _defaultShaderProgram = nullptr;
}

ShaderManager::~ShaderManager() {
}

IShaderProgram* ShaderManager::CreateSimpleShaderProgram(std::string const& vertexShaderCode, std::string const& fragmentShaderCode) {
    IShader* sh[2] {
        CreateAndCompile(IShader::Type::Vertex, vertexShaderCode),
        CreateAndCompile(IShader::Type::Fragment, fragmentShaderCode)
    };

    //Delete shaders if one of them isn't compiled.
    bool compiled = true;
    for(unsigned char i = 0; i < 2; ++i) {
        if(sh[i] == nullptr) compiled = false;
        else if(!compiled) {
            sh[i]->Destroy();
            delete sh[i];
        }
    }
    if(!compiled) {
        mr::Log::LogString("Failed ShaderManager::CreateSimpleShaderProgram. Shaders not compiled.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IShaderProgram* sp = CreateAndLink({sh[0], sh[1]});
    if(!sp) {
        //sp is nullptr.
        //Don't exit from this point, because we need to free shaders.
        mr::Log::LogString("Failed ShaderManager::CreateSimpleShaderProgram. Shader program not linked.", MR_LOG_LEVEL_ERROR);
    }
    else RegisterShaderProgram(sp);

    //Free shaders. Now it's not needed.
    for(unsigned char i = 0; i < 2; ++i) {
        sh[i]->Destroy();
        delete sh[i];
    }

    return sp;
}

IShaderProgram* ShaderManager::CreateDefaultShaderProgram() {
    IShaderProgram* sp =
        CreateSimpleShaderProgram(
            mr::BaseShaderVS_glsl_s,
            mr::BaseShaderFS_glsl_s
        );
    if(!sp) mr::Log::LogString("Failed ShaderManager::CreateDefaultShaderProgram. Shader program not created.", MR_LOG_LEVEL_ERROR);
    else {
        RegisterShaderProgram(sp);
        CompleteShaderProgram(sp);
    }
    return sp;
}

IShaderProgram* ShaderManager::DefaultShaderProgram() {
    if(_defaultShaderProgram != nullptr) return _defaultShaderProgram;
    _defaultShaderProgram = CreateDefaultShaderProgram();
    return _defaultShaderProgram;
}

IShaderProgram* ShaderManager::CreateShaderProgramFromCache(ShaderProgramCache cache) {
    if(cache.format == 0) {
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgramFromCache. Invalid cache format.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }
    if(cache.data.GetNum() == 0) {
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgramFromCache. Invalid cache data.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IShaderProgram* ishaderProgram = CreateShaderProgram();
    if(!ishaderProgram) {
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgramFromCache. Shader program not created.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    unsigned int handle = ishaderProgram->GetGPUHandle();

#ifdef MR_CHECK_LARGE_GL_ERRORS
    std::string gl_str = "";
    mr::gl::ClearError();
#endif
     glProgramBinary(handle, cache.format, cache.data.GetArray(), cache.data.GetNum());
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::gl::CheckError(&gl_str, 0)) {
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgramFromCache. Failed loading shader program binary. " + gl_str, MR_LOG_LEVEL_ERROR);
        ishaderProgram->Destroy();
        delete ishaderProgram;
        return nullptr;
    }
#endif

    int link_status = -1;
    glGetProgramiv(handle, GL_LINK_STATUS, &link_status);
    if(link_status != GL_TRUE){
        mr::Log::LogString("Failed ShaderManager::CreateShaderProgramFromCache. Shader program linking failed.", MR_LOG_LEVEL_ERROR);
        ishaderProgram->Destroy();
        delete ishaderProgram;
        return nullptr;
    }

    ShaderProgram* shaderProgram = static_cast<ShaderProgram*>(ishaderProgram);
    shaderProgram->_linked = true;
    return ishaderProgram;
}

bool ShaderManager::CompleteShaderProgram(IShaderProgram* shaderProgram) {
    if(shaderProgram == nullptr || shaderProgram->GetGPUHandle() == 0) {
        mr::Log::LogString("Failed ShaderManager::CompleteShaderProgram. Shader program is null or not created.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    //Need enumerator for ublocks
    mu::ArrayHandle<ShaderUniformBlockInfo> ublocks = shaderProgram->GetMap()->GetUniformBlocks();
    for(size_t i = 0; i < ublocks.GetNum(); ++i) {
        if(ublocks.GetArray()[i].name == MR_SHADER_TEXTURE_BLOCK) {
            shaderProgram->GetMap()->GetUniformBlock(ublocks.GetArray()[i].name).binding = MR_SHADER_TEXTURE_BLOCK_BINDING;
            glUniformBlockBinding(shaderProgram->GetGPUHandle(), ublocks.GetArray()[i].location, MR_SHADER_TEXTURE_BLOCK_BINDING);
        } else if(ublocks.GetArray()[i].name == MR_SHADER_POINTLIGHTS_BLOCK) {
            shaderProgram->GetMap()->GetUniformBlock(ublocks.GetArray()[i].name).binding = MR_SHADER_POINTLIGHTS_BLOCK_BINDING;
            glUniformBlockBinding(shaderProgram->GetGPUHandle(), ublocks.GetArray()[i].location, MR_SHADER_POINTLIGHTS_BLOCK_BINDING);
        }
    }

    return true;
}

void ShaderManager::RegisterShaderProgram(IShaderProgram* shaderProgram) {
    _programs.insert(shaderProgram);
}

void ShaderManager::UnRegisterShaderProgram(IShaderProgram* shaderProgram) {
    _programs.erase(shaderProgram);
}

}
