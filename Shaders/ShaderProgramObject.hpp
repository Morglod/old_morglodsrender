#pragma once

#include "ShaderInterfaces.hpp"
#include <unordered_map>

namespace mr {

class ShaderProgram : public IShaderProgram {
    friend class ShaderManager;
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
    mu::ArrayHandle<IShaderUniform*> GetShaderUniforms() override;
    mu::ArrayHandle<ShaderUniformInfo> GetCompiledUniforms() override;
    bool IsUniform(std::string const& uniformName) override;

    void UpdateUniforms() override;

    inline bool IsLinked() override { return _linked; }

    ShaderProgramCache GetCache() override;

    //ObjectHandle
    void Destroy() override;

    virtual ~ShaderProgram();
protected:
    bool Create() override;

    ShaderProgram();
    //std::vector<IShaderUniform*> _shaderUniforms;
    std::unordered_map<std::string, IShaderUniform*> _shaderUniforms; // (uniformName, uniformHandle)
    bool _linked;
};

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
