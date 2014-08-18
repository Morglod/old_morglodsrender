#pragma once

#ifndef _MR_SHADER_OBJECTS_H_
#define _MR_SHADER_OBJECTS_H_

#include "../Config.hpp"
#include "../Utils/Events.hpp"
#include "ShaderInterfaces.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

class Shader : public IShader {
public:
    inline const unsigned int& GetGPUHandle() override { return _gpu_handle; }
    inline const IShader::Type& GetType() override { return _type; }

    bool Attach(IShaderProgram* program) override;
    void Detach(IShaderProgram* program) override;
    StaticArray<IShaderProgram*> GetConnectedPrograms() override;

    //ObjectHandle
    void Destroy() override;

    Shader(const unsigned int& gpu_handle, const IShader::Type& type);
    virtual ~Shader();

    static Shader* Create(const IShader::Type& type);
protected:
    IShader::Type _type;
    unsigned int _gpu_handle;
    std::vector<IShaderProgram*> _connected;
};

class ShaderProgram : public IShaderProgram {
public:
    bool BindDefaultShaderInOut() override;

    IShaderUniform* CreateUniform(const std::string& name, const MR::IShaderUniform::Type& type, void* value) override;
    inline IShaderUniform* CreateUniform(const std::string& name, int* value);
    inline IShaderUniform* CreateUniform(const std::string& name, float* value);
    inline IShaderUniform* CreateUniform(const std::string& name, glm::vec2* value);
    inline IShaderUniform* CreateUniform(const std::string& name, glm::vec3* value);
    inline IShaderUniform* CreateUniform(const std::string& name, glm::vec4* value);
    inline IShaderUniform* CreateUniform(const std::string& name, glm::mat4* value);

    void SetUniform(const std::string& name, const int& value) override;
    void SetUniform(const std::string& name, const float& value) override;
    void SetUniform(const std::string& name, const glm::vec2& value) override;
    void SetUniform(const std::string& name, const glm::vec3& value) override;
    void SetUniform(const std::string& name, const glm::vec4& value) override;
    void SetUniform(const std::string& name, const glm::mat4& value) override;

    void DeleteUniform(IShaderUniform* su) override;
    IShaderUniform* FindShaderUniform(const std::string& name) override;
    size_t GetShaderUniformsPtr(IShaderUniform*** list_ptr) override;
    StaticArray<ShaderUniformInfo> GetCompiledUniforms() override;

    inline unsigned int GetGPUHandle() override { return _program; }
    inline MR::IShaderProgram::Features GetFeatures() override { return _features; }

    void UpdateUniforms() override;

    //Usable
    bool Use() override;

    //ObjectHandle
    void Destroy() override;

    ShaderProgram(const unsigned int& gpu_program, const MR::IShaderProgram::Features& features);
    virtual ~ShaderProgram();

    static ShaderProgram* Create();

protected:
    unsigned int _program; //OpenGL shader program
    std::vector<IShaderUniform*> _shaderUniforms;
    MR::IShaderProgram::Features _features;

    //std::vector<IShader*> _shaders;
    //std::vector<IShaderUniformBlock*> _shaderUniformBlocks;
};
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, int* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Int, value);
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, float* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Float, value);
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec2* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Vec2, value);
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec3* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Vec3, value);
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec4* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Vec4, value);
}

MR::IShaderUniform* MR::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::mat4* value) {
    return CreateUniform(uniform_name, MR::IShaderUniform::SUT_Mat4, value);
}

#endif
