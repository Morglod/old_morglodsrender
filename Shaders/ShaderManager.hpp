#pragma once

#include "ShaderInterfaces.hpp"
#include "ShaderUniforms.hpp"

#include <mu/Singleton.hpp>
#include <mu/Containers.hpp>
#include <unordered_map>
#include <unordered_set>

namespace mr {

class IGPUBuffer;
class IShaderCompiler;

class ShaderManager : public mu::Singleton<ShaderManager> {
public:
    inline void SetGlobalUniform(std::string const& name, IShaderUniformRef::Type const& type, const void* value, bool oneTime = false) {
        SetGlobalUniform(ShaderUniformDesc(name, type), value, oneTime);
    }

    virtual void SetGlobalUniform(ShaderUniformDesc const& desc, const void* value, bool oneTime = false);
    virtual void DeleteGlobalUniform(ShaderUniformDesc const& desc);

    virtual void UpdateGlobalUniform(std::string const& name);
    virtual void UpdateGlobalUniform(std::string const& name, const void* value);
    virtual void UpdateAllGlobalUniforms();

    virtual void SetUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer);
    virtual void RemoveUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer);

    virtual void SetNVVBUMPointer(int const& vertexAttributeLocation, uint64_t const& residentPointer);

    inline void SetCompiler(IShaderCompiler* shaderCompiler) { _shaderCompiler = shaderCompiler; }
    inline IShaderCompiler* GetCompiler() { return _shaderCompiler; }

    virtual IShader* CreateShader(ShaderType const& type);
    virtual IShader* CreateAndCompile(ShaderType const& type, std::string const& code);

    virtual IShaderProgram* CreateShaderProgram();
    virtual IShaderProgram* CreateShaderProgramFromCache(ShaderProgramCache cache);
    virtual IShaderProgram* CreateSimpleShaderProgram(std::string const& vertexShaderCode, std::string const& fragmentShaderCode);
    virtual IShaderProgram* CreateAndLink(mu::ArrayHandle<IShader*> shaders);

    virtual IShaderProgram* CreateDefaultShaderProgram();
    virtual IShaderProgram* DefaultShaderProgram();

    virtual bool CompleteShaderProgram(IShaderProgram* shaderProgram); //Set default bindings, etc

    virtual void RegisterShaderProgram(IShaderProgram* shaderProgram);
    virtual void UnRegisterShaderProgram(IShaderProgram* shaderProgram);

    ShaderManager();
    virtual ~ShaderManager();
protected:

    IShaderCompiler* _shaderCompiler;

    std::unordered_map<std::string, const void*> _globalUniforms; //<name, data>
    std::unordered_map<std::string, ShaderUniformDesc> _globalUniformsDesc; //<name, desc>
    std::unordered_map<unsigned int, IGPUBuffer*> _globalUbos;

    IShaderProgram* _defaultShaderProgram;

    std::unordered_set<IShaderProgram*> _programs;
};

}
