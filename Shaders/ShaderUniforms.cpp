#include "ShaderUniforms.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Log.hpp"
#include "../Utils/Debug.hpp"
#include "../StateCache.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

#include <iostream>

namespace mr {

ShaderUniformDesc::ShaderUniformDesc() : name("NoNameUniform"), type(IShaderUniformRef::Type::Float) {
}

ShaderUniformDesc::ShaderUniformDesc(std::string const& n, IShaderUniformRef::Type const& t) : name(n), type(t) {
}

ShaderUniformInfo::ShaderUniformInfo() : name("NoNameUniform"), gl_type(0), location(-1) {
}

ShaderUniformInfo::ShaderUniformInfo(const std::string& n, const unsigned int & t, int const& loc)
: name(n), gl_type(t), location(loc) {}

void ShaderUniformBlockInfo::_ResetHash() {
    static std::hash<std::string> str_hash;
    const size_t num = uniform_names.GetNum();
    std::string* names = uniform_names.GetArray();
    size_t* hash_names = new size_t[num];
    uniform_hash_names = mu::ArrayHandle<size_t>(hash_names, num, true, false);
    for(size_t i = 0; i < num; ++i) {
        hash_names[i] = str_hash(names[i]);
    }
}

ShaderUniformBlockInfo::ShaderUniformBlockInfo() : name("NoNameUniformBlock"), location(-1) {
}

ShaderUniformBlockInfo::ShaderUniformBlockInfo(std::string const& name_, int location_, mu::ArrayHandle<std::string> const& uniformNames_, mu::ArrayHandle<int> const& uniformOffsets_)
: name(name_), uniform_names(uniformNames_), uniform_hash_names(), uniform_offsets(uniformOffsets_), location(location_) {
    _ResetHash();
}

ShaderUniformBlockInfo::ShaderUniformBlockInfo(std::string const& name_, int location_, std::initializer_list<std::pair<std::string, int>> const& init_list)
: name(name_), location(location_) {
    std::string* uniform_names_ar = new std::string[init_list.size()];
    int* uniform_offsets_ar = new int[init_list.size()];
    uniform_names = mu::ArrayHandle<std::string>(uniform_names_ar, init_list.size(), true, false);
    uniform_offsets = mu::ArrayHandle<int>(uniform_offsets_ar, init_list.size(), true, false);

    size_t i = 0;
    for(std::pair<std::string, int> const& p : init_list) {
        uniform_names_ar[i] = p.first;
        uniform_offsets_ar[i] = p.second;
        ++i;
    }
    _ResetHash();
}

IShaderUniformRef* ShaderUniformMap::CreateRef(std::string const& name, mr::IShaderUniformRef::Type const& type, void* value) {
    if(!IsUniform(name)) return nullptr;

    ShaderUniformInfo* info = &_uniforms[name];
    if(info->value_ref != nullptr) {
        DeleteRef(name);
    }
    ShaderUniformRef* uref = new ShaderUniformRef(name, type, info->location, value, this);
    IShaderUniformRef* iuref = static_cast<IShaderUniformRef*>(uref);
    info->value_ref = iuref;
    _refs.insert(iuref);
    iuref->Update();
    return iuref;
}

void ShaderUniformMap::DeleteRef(std::string const& name) {
    if(!IsUniform(name)) return;

    ShaderUniformInfo* info = &_uniforms[name];
    if(info->value_ref == nullptr) return;

    _refs.erase(info->value_ref);
    delete info->value_ref;
    info->value_ref = nullptr;
}

IShaderUniformRef* ShaderUniformMap::FindRef(std::string const& name) {
    if(!IsUniform(name)) return nullptr;
    return _uniforms[name].value_ref;
}

mu::ArrayHandle<IShaderUniformRef*> ShaderUniformMap::GetRefs() {
    IShaderUniformRef** refs = new IShaderUniformRef*[_refs.size()];
    size_t i = 0;
    for(IShaderUniformRef* ur : _refs) {
        refs[i] = ur;
        ++i;
    }
    return mu::ArrayHandle<IShaderUniformRef*>(refs, _refs.size(), true, false);
}

mu::ArrayHandle<ShaderUniformInfo> ShaderUniformMap::GetCompiledUniforms() {
    ShaderUniformInfo* info = new ShaderUniformInfo[_uniforms.size()];
    size_t i = 0;
    for(auto const& inf_pair : _uniforms) {
        info[i] = inf_pair.second;
        ++i;
    }
    return mu::ArrayHandle<ShaderUniformInfo>(info, _uniforms.size(), true, false);
}

bool ShaderUniformMap::IsUniform(std::string const& uniformName) {
    return (_uniforms.count(uniformName) != 0);
}

int ShaderUniformMap::GetUniformGPULocation(std::string const& uniformName) {
    if(!IsUniform(uniformName)) return -1;
    if(_uniforms[uniformName].location == -1) {
        if(!_GetUniformGPULocation(uniformName, &(_uniforms[uniformName].location))) {
            mr::Log::LogString("Failed ShaderUniformMap::GetUniformGPULocation.", MR_LOG_LEVEL_ERROR);
        }
    }
    return _uniforms[uniformName].location;
}

void ShaderUniformMap::DeleteAllRefs() {
    while(!_refs.empty()) {
        DeleteRef((*(_refs.begin()))->GetName());
    }
}

bool ShaderUniformMap::_GetUniformGPULocation(std::string const& uniformName, int* out) {
/*#ifdef MR_CHECK_SMALL_GL_ERRORS
    int gl_er = 0;
    mr::gl::ClearError();
#endif*/

    *out = glGetUniformLocation(_program->GetGPUHandle(), uniformName.c_str());

/*#ifdef MR_CHECK_SMALL_GL_ERRORS
    if(mr::gl::CheckError(0, &gl_er)) {
        std::string err_str = "Error in ShaderUniformMap::_GetUniformGPULocation : glGetUniformLocation("+std::to_string(_program->GetGPUHandle())+", \""+uniformName+"\") ended with \"" + std::to_string(gl_er) + "\" code. ";
        switch(gl_er) {
        case GL_INVALID_VALUE:
            err_str += "Shader program id is not generated by OpenGL values."; break;
        case GL_INVALID_OPERATION:
            err_str += "id is not an OpenGL program object or program is not successfully linked."; break;
        default:
            err_str += "Unknow code."; break;
        }
        mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif*/

    if((*out) == -1) {
        mr::Log::LogString("Error in ShaderUniformMap::_GetUniformGPULocation : glGetUniformLocation("+std::to_string(_program->GetGPUHandle())+", \""+uniformName+"\") == -1.", MR_LOG_LEVEL_ERROR);
/*#ifdef MR_CHECK_SMALL_GL_ERRORS
        mr::Log::LogString("Error in ShaderUniformMap::_GetUniformGPULocation : glGetUniformLocation("+std::to_string(_program->GetGPUHandle())+", \""+uniformName+"\") == -1.", MR_LOG_LEVEL_ERROR);
#endif*/
        return false;
    }

    return true;
}

void ShaderUniformMap::SetUniform(int const& location, int const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform1i(_program->GetGPUHandle(), location, value);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform1i(location, value);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, unsigned int const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform1ui(_program->GetGPUHandle(), location, value);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform1ui(location, value);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, float const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform1f(_program->GetGPUHandle(), location, value);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform1f(location, value);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, glm::vec2 const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform2fv(_program->GetGPUHandle(), location, 1, &value.x);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform2fv(location, 1, &value.x);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, glm::vec3 const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform3fv(_program->GetGPUHandle(), location, 1, &value.x);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform3fv(location, 1, &value.x);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, glm::vec4 const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniform4fv(_program->GetGPUHandle(), location, 1, &value.x);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniform4fv(location, 1, &value.x);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, glm::mat4 const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniformMatrix4fv(_program->GetGPUHandle(), location, 1, false, &value[0][0]);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniformMatrix4fv(location, 1, false, &value[0][0]);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetUniform(int const& location, uint64_t const& value) {
    if(mr::gl::IsDSA_EXT()) glProgramUniformui64NV(_program->GetGPUHandle(), location, value);
    else {
        StateCache* cache = StateCache::GetDefault();
        IShaderProgram* was = nullptr;
        if(cache->ReSetShaderProgram(_program, &was)) {
            glUniformui64NV(location, value);
        } else {
            mr::Log::LogString("Failed ShaderUniformMap::SetUniform. Failed set shader program.", MR_LOG_LEVEL_ERROR);
        }
        cache->SetShaderProgram(_program);
    }
}

void ShaderUniformMap::SetNVVBUMPointer(int const& vertexAttributeLocation, uint64_t const& residentPointer) {
    /*
    https://www.opengl.org/registry/specs/NV/shader_buffer_load.txt
    and
    NvidiaGameWorks/gl_samples_2_11/BindlessApp example
    */
    glVertexAttribI2i(vertexAttributeLocation,
                    (int)(residentPointer & 0xFFFFFFFF),
                    (int)((residentPointer >> 32) & 0xFFFFFFFF));
}

void ShaderUniformMap::Reset(bool saveRefs) {
    unsigned int handle = _program->GetGPUHandle();

    int act_uniform_blocks = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORM_BLOCKS, &act_uniform_blocks);
    if(act_uniform_blocks == 0) {
        _uniformBlocks.clear();
    }

    std::vector<int> ub_inds; //not just normal uniform, so filter it, when uniforms parsing.

    for(int iu = 0; iu < act_uniform_blocks; ++iu) {
        char namebuffer[1024];
        int namebuffer_size = 0;
        glGetActiveUniformBlockName(handle, iu, 1024, &namebuffer_size, &namebuffer[0]);
        std::string name(namebuffer, namebuffer_size);

        _uniformBlocks[name].name = name;
        _uniformBlocks[name].location = iu;

        int ub_uniforms_num = 0;
        glGetActiveUniformBlockiv(handle, iu, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &ub_uniforms_num);

        int ub_size = 0;
        glGetActiveUniformBlockiv(handle, iu, GL_UNIFORM_BLOCK_DATA_SIZE, &ub_size);

        _uniformBlocks[name].bufferSize = ub_size;

        if(ub_uniforms_num > 0) {
            ub_inds.reserve(ub_uniforms_num);

            unsigned int ub_uniforms_inds[ub_uniforms_num];
            glGetActiveUniformBlockiv(handle, iu, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, (int*)ub_uniforms_inds);

            int* ub_uniforms_offsets = new int[ub_uniforms_num];
            glGetActiveUniformsiv(handle, ub_uniforms_num, ub_uniforms_inds, GL_UNIFORM_OFFSET, ub_uniforms_offsets);
            _uniformBlocks[name].uniform_offsets = mu::ArrayHandle<int>(ub_uniforms_offsets, ub_uniforms_num, true, false);

            std::string* u_names = new std::string[ub_uniforms_num];
            _uniformBlocks[name].uniform_names = mu::ArrayHandle<std::string>(u_names, ub_uniforms_num, true, false);

            for(int i = 0; i < ub_uniforms_num; ++i) {
                ub_inds.push_back(ub_uniforms_inds[i]);
                char u_namebuf[1024];
                int u_name_size = 0;
                int unif_size = 0;
                unsigned int unif_type = 0;
                glGetActiveUniform(handle, ub_uniforms_inds[i], 1024, &u_name_size, &unif_size, &unif_type, &u_namebuf[0]);
                u_names[i] = std::string(u_namebuf, u_name_size);
            }
        }

        _uniformBlocks[name]._ResetHash();
    }

    int act_uniforms = 0;
    glGetProgramiv(handle, GL_ACTIVE_UNIFORMS, &act_uniforms);
    if(act_uniforms == 0) {
        if(!saveRefs) DeleteAllRefs();
        _uniforms.clear();
    }

    for(int iu = 0; iu < act_uniforms; ++iu) {
        if(std::find(ub_inds.begin(), ub_inds.end(), iu) != ub_inds.end()) continue; //Its in uniform block

        char namebuffer[1024];
        int real_buf_size = 0;
        int unif_size = 0;
        unsigned int uni_type = 0;
        glGetActiveUniform(handle, iu, 1024, &real_buf_size, &unif_size, &uni_type, &namebuffer[0]);
        std::string name(namebuffer);
        int location = -1;
        if(!_GetUniformGPULocation(name, &location)) {
            mr::Log::LogString("In ShaderUniformMap::Reset. Failed get uniform gpu location for \""+name+"\".", MR_LOG_LEVEL_WARNING);
            location = -1;
        }
        _uniforms[name] =  ShaderUniformInfo(name, uni_type, location);
    }

    //remap refs
    if(saveRefs) {
        std::unordered_set<IShaderUniformRef*> refs = _refs;
        _refs.clear();
        for(IShaderUniformRef* r : refs) {
            if(IsUniform(r->GetName())) {
                void* value = r->GetValuePtr();
                IShaderUniformRef::Type type = r->GetType();
                std::string name = r->GetName();
                CreateRef(name, type, value);
            }
        }
        std::unordered_set<IShaderUniformRef*> tmp = _refs;
        _refs = refs;
        DeleteAllRefs();
        _refs = tmp;
    }
}

void ShaderUniformMap::UpdateRefs() {
    for(IShaderUniformRef* uref : _refs) {
        void* value = uref->GetValuePtr();
        if(value) {
/*#ifdef MR_CHECK_SMALL_GL_ERRORS
            int gl_er = 0;
            mr::gl::ClearError();
#endif*/
            uref->Update();
/*#ifdef MR_CHECK_SMALL_GL_ERRORS
            if(mr::gl::CheckError(0, &gl_er)) {
                std::string err_str = "Error in ShaderUniformMap::Update GL \""+ std::to_string(gl_er) + "\" code. ";
                switch(gl_er) {
                case GL_INVALID_VALUE:
                    err_str += "Uniform's data is less than zero."; break;
                case GL_INVALID_OPERATION:
                    err_str += "No current shader program or uniforms size doesn't match or uniforms type doesn't match or invalid uniform location or bad sampler."; break;
                default:
                    err_str += "Unknow code."; break;
                }
                mr::Log::LogString(err_str, MR_LOG_LEVEL_ERROR);
            }
#endif*/
        }
    }
}

ShaderUniformMap::ShaderUniformMap(IShaderProgram* program) : _program(program) {
}

void ShaderUniformRef::Update() {}
ShaderUniformRef::ShaderUniformRef(std::string const&, IShaderUniformRef::Type const&, int const& ,void*, ShaderUniformMap*) {
}

ShaderUniformRef::~ShaderUniformRef() {
}

}
