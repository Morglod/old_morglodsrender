#pragma once

#ifndef _MR_SHADER_UNIFORMS_H_
#define _MR_SHADER_UNIFORMS_H_

#include "ShaderInterfaces.hpp"
#include "../Utils/Log.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

#include <unordered_map>
#include <unordered_set>
#include <initializer_list>

namespace mr {

struct ShaderUniformInfo {
    std::string name;
    unsigned int gl_type;
    IShaderUniformRef* value_ref; //may be nullptr
    int location;

    ShaderUniformInfo();
    ShaderUniformInfo(std::string const& name, unsigned int const& type, int const& loc);
};

struct ShaderUniformBlockInfo {
    std::string name;
    mu::ArrayHandle<std::string> uniform_names;
    mu::ArrayHandle<size_t> uniform_hash_names;
    mu::ArrayHandle<int> uniform_offsets;
    int location = -1;
    int bufferSize = -1;
    int binding = -1;

    inline int GetOffset(std::string const& name) {
        static std::hash<std::string> str_hash;
        size_t* hash_ar = uniform_hash_names.GetArray();
        size_t name_hash = str_hash(name);
        for(size_t i = 0; i < uniform_hash_names.GetNum(); ++i) {
            if(name_hash == hash_ar[i]) {
                return uniform_offsets.GetArray()[i];
            }
        }
        mr::Log::LogString("Failed ShaderUniformBlockInfo::GetOffset. Failed get offset of \""+name+"\" not found in active uniforms.", MR_LOG_LEVEL_ERROR);
        return -1;
    }

    ShaderUniformBlockInfo();
    ShaderUniformBlockInfo(std::string const& name, int location, mu::ArrayHandle<std::string> const& uniformNames, mu::ArrayHandle<int> const& uniformOffsets);
    ShaderUniformBlockInfo(std::string const& name, int location, std::initializer_list<std::pair<std::string, int>> const& init_list);

    void _ResetHash();
};

struct ShaderUniformDesc {
    std::string name;
    IShaderUniformRef::Type type;

    ShaderUniformDesc();
    ShaderUniformDesc(std::string const& name, IShaderUniformRef::Type const& type);
};

///Set uniform values and handle refs.
class ShaderUniformMap {
public:
    virtual IShaderUniformRef* CreateRef(std::string const& name, IShaderUniformRef::Type const& type, void* value);
    virtual void DeleteRef(std::string const& name);
    virtual IShaderUniformRef* FindRef(std::string const& name);
    virtual mu::ArrayHandle<IShaderUniformRef*> GetRefs();
    virtual void DeleteAllRefs();

    virtual mu::ArrayHandle<ShaderUniformInfo> GetUniforms();
    virtual bool IsUniform(std::string const& uniformName);
    virtual int GetUniformGPULocation(std::string const& uniformName);

    virtual mu::ArrayHandle<ShaderUniformBlockInfo> GetUniformBlocks();
    virtual inline ShaderUniformBlockInfo& GetUniformBlock(std::string const& name) { return _uniformBlocks[name]; }
    virtual inline int GetUniformBlockGPULocation(std::string const& name) { return _uniformBlocks[name].location; }

    virtual void Reset(bool saveRefs); //reset uniforms map

    virtual void UpdateRefs();

    inline virtual IShaderProgram* GetShaderProgram() { return _program; }

    template<typename T>
    inline void SetUniformT(std::string const& name, T const& value) {
        SetUniformT(GetUniformGPULocation(name), value);
    }

    template<typename T>
    inline void SetUniformT(int location, T const& value) {
        SetUniform(location, value);
    }

    template<typename T>
    inline void SetUniformT(int location, T* valuePtr) {
        SetUniform(location, *valuePtr);
    }

    inline void SetUniform(int location, IShaderUniformRef::Type const& type, const void* value) {
        switch(type){
        case IShaderUniformRef::Type::Float:
            SetUniformT(location, (float*)value);
            break;
        case IShaderUniformRef::Type::Sampler1D:
        case IShaderUniformRef::Type::Sampler2D:
        case IShaderUniformRef::Type::Sampler3D:
        case IShaderUniformRef::Type::Int:
            SetUniformT(location, (int*)value);
            break;
        case IShaderUniformRef::Type::Mat4:
            SetUniformT(location, (glm::mat4*)value);
            break;
        case IShaderUniformRef::Type::Vec2:
            SetUniformT(location, (glm::vec2*)value);
            break;
        case IShaderUniformRef::Type::Vec3:
            SetUniformT(location, (glm::vec3*)value);
            break;
        case IShaderUniformRef::Type::Vec4:
            SetUniformT(location, (glm::vec4*)value);
            break;
        case IShaderUniformRef::Type::UInt64:
            SetUniformT(location, (uint64_t*)value);
            break;
        }
    }

    virtual void SetUniform(int const& location, int const& value);
    virtual void SetUniform(int const& location, unsigned int const& value);
    virtual void SetUniform(int const& location, float const& value);
    virtual void SetUniform(int const& location, glm::vec2 const& value);
    virtual void SetUniform(int const& location, glm::vec3 const& value);
    virtual void SetUniform(int const& location, glm::vec4 const& value);
    virtual void SetUniform(int const& location, glm::mat4 const& value);
    virtual void SetUniform(int const& location, uint64_t const& value);

    virtual void SetNVVBUMPointer(int const& vertexAttributeLocation, uint64_t const& residentPointer);

    ShaderUniformMap(IShaderProgram* program);
protected:
    virtual bool _GetUniformGPULocation(std::string const& uniformName, int* out); //not cached, get directly from gl

    std::unordered_map<std::string, ShaderUniformInfo> _uniforms;
    std::unordered_set<IShaderUniformRef*> _refs;
    std::unordered_map<std::string, ShaderUniformBlockInfo> _uniformBlocks;
    IShaderProgram* _program;
};

/** Named uniform ref in shader program
 *  After shader recompilation, call ShaderUniform::ResetLocation method
 */
class ShaderUniformRef : public IShaderUniformRef {
public:
    inline std::string GetName() override { return _name; }
    inline IShaderUniformRef::Type GetType() override { return _type; }

    inline void SetValuePtr(void* ptr) override { _value = ptr; }
    inline void* GetValuePtr() override { return _value; }

    inline int GetGPULocation() override { return _gpu_location; }

    void Update() override;

    inline ShaderUniformMap* GetMap() override { return _owner; }

    ShaderUniformRef(std::string const& name, IShaderUniformRef::Type const& type, int const& gpuLocation, void* value, ShaderUniformMap* map);
    virtual ~ShaderUniformRef();
protected:
    std::string _name;
    int _gpu_location;
    IShaderUniformRef::Type _type;
    void* _value;
    ShaderUniformMap* _owner;
};

}

#endif
