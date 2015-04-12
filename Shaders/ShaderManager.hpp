#pragma once

#include "ShaderInterfaces.hpp"

#include <Singleton.hpp>
#include <Containers.hpp>
#include <unordered_map>

namespace mr {

class IGPUBuffer;
class IShaderCompiler;

class ShaderManager : public mu::Singleton<ShaderManager> {
public:
    virtual void SetGlobalUniform(ShaderUniformDesc const& desc, void* data);
    virtual void RemoveGlobalUniform(ShaderUniformDesc const& desc);

    virtual void SetUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer);
    virtual void RemoveUniformBufferObject(unsigned int const& index, IGPUBuffer* buffer);

    inline void SetCompiler(IShaderCompiler* shaderCompiler) { _shaderCompiler = shaderCompiler; }
    inline IShaderCompiler* GetCompiler() { return _shaderCompiler; }

    virtual IShader* CreateShader(IShader::Type const& type);
    virtual IShader* CreateAndCompile(IShader::Type const& type, std::string const& code);

    virtual IShaderProgram* CreateShaderProgram();
    virtual IShaderProgram* CreateShaderProgramFromCache(ShaderProgramCache cache);
    virtual IShaderProgram* CreateSimpleShaderProgram(std::string const& vertexShaderCode, std::string const& fragmentShaderCode);
    virtual IShaderProgram* CreateAndLink(mu::ArrayHandle<IShader*> shaders);

    virtual IShaderProgram* CreateDefaultShaderProgram();
    virtual IShaderProgram* DefaultShaderProgram();

    ShaderManager();
    virtual ~ShaderManager();
protected:
    IShaderCompiler* _shaderCompiler;

    std::unordered_map<std::string, void*> _globalUniforms; //<name, data>
    std::unordered_map<std::string, ShaderUniformDesc> _globalUniformsDesc; //<name, desc>
    std::unordered_map<unsigned int, IGPUBuffer*> _globalUbos;

    IShaderProgram* _defaultShaderProgram;
};

}
