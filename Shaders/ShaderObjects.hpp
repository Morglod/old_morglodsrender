#pragma once

#ifndef _MR_SHADER_OBJECTS_H_
#define _MR_SHADER_OBJECTS_H_

#include "ShaderInterfaces.hpp"

namespace mr {

class Shader : public IShader {
public:
    inline IShader::Type GetType() override { return _type; }
    inline bool IsCompiled() override { return _compiled; }
    bool Compile(IShader::Type const& type, std::string const& code) override;

    //ObjectHandle
    void Destroy() override;

    Shader();
    virtual ~Shader();

    static Shader* CreateAndCompile(const IShader::Type& type, const std::string& code);
protected:
    IShader::Type _type;
    bool _compiled;
};

class ShaderProgram : public IShaderProgram {
public:
    IShaderUniform* CreateUniform(const std::string& name, const mr::IShaderUniform::Type& type, void* value) override;
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
    TStaticArray<IShaderUniform*> GetShaderUniforms() override;
    TStaticArray<ShaderUniformInfo> GetCompiledUniforms() override;
    bool IsUniform(std::string const& uniformName) override;

    void UpdateUniforms() override;

    bool Link(TStaticArray<IShader*> shaders) override;
    inline bool IsLinked() override { return _linked; }

    ShaderProgramCache GetCache() override;

    //Usable
    bool Use() override;

    //ObjectHandle
    void Destroy() override;

    ShaderProgram();
    virtual ~ShaderProgram();

    static ShaderProgram* CreateAndLink(TStaticArray<IShader*> shaders);
    static ShaderProgram* Default();
    static ShaderProgram* DefaultBase(); //for default material flag
    static ShaderProgram* DefaultWithTexture(); //Texture uniform "MR_SHADER_COLOR_TEX"
    static ShaderProgram* FromCache(ShaderProgramCache cache);

protected:
    std::vector<IShaderUniform*> _shaderUniforms;
    bool _linked;
};

void UseNullShaderProgram();
void DestroyAllShaderPrograms();
IShaderProgram* GetUsedShaderProgram();

}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, int* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Int, value);
}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, float* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Float, value);
}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec2* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Vec2, value);
}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec3* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Vec3, value);
}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::vec4* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Vec4, value);
}

mr::IShaderUniform* mr::ShaderProgram::CreateUniform(const std::string& uniform_name, glm::mat4* value) {
    return CreateUniform(uniform_name, mr::IShaderUniform::Mat4, value);
}

#endif
