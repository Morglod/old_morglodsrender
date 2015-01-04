#pragma once

#ifndef _MR_SHADER_INTERFACES_H_
#define _MR_SHADER_INTERFACES_H_

#include "../CoreObjects.hpp"

namespace mr {

class IShaderProgram;

class IShaderUniform {
    friend class IShaderProgram;
public:
    enum Type {
        Float = 0,
        Vec2 = 1, //glm::vec2 float[2]
        Vec3 = 2, //glm::vec3 float[3]
        Vec4 = 3, //glm::vec4 float[4]
        Mat4 = 4, //glm::mat4 float[4][4]
        Int = 5, //int
        Sampler1D, //int
        Sampler2D, //int
        Sampler3D  //int
    };

    static std::string TypeToString(const IShaderUniform::Type& t);

    mu::Event<IShaderUniform*, void*> OnNewValuePtr;
    mu::Event<IShaderUniform*, IShaderProgram*, const int&> OnNewLocation; //as args (shader program, new uniform location)

    virtual std::string GetName() = 0;
    virtual IShaderUniform::Type GetType() = 0;
    virtual void SetPtr(void* ptr)  = 0;
    virtual void* GetPtr() = 0;
    virtual int GetGPULocation() = 0;
    virtual bool ResetLocation() = 0;
    virtual void Update() = 0; //Update value
    virtual IShaderProgram* GetParent() = 0;

    virtual ~IShaderUniform() {}
};

struct ShaderUniformInfo {
public:
    IShaderProgram* program;
    std::string name;
    int uniform_size;
    unsigned int uniform_gl_type;

    ShaderUniformInfo();
    ShaderUniformInfo(IShaderProgram* p, const std::string& n, const int& s, const unsigned int & t);
};

class ShaderUniformsList {
public:
    inline mr::TStaticArray<IShaderUniform*> GetList() { return _uniforms; }
    inline void SetList(mr::TStaticArray<IShaderUniform*> const& uniforms) { _uniforms = uniforms; }

    inline void Update() {
        IShaderUniform** ptr = _uniforms.GetRaw();
        for(size_t i = 0; i < _uniforms.GetNum(); ++i){
            ptr[i]->Update();
        }
    }

    ShaderUniformsList();
    ShaderUniformsList(mr::TStaticArray<IShaderUniform*> const& uniforms);
    ShaderUniformsList(mr::TDynamicArray<IShaderUniform*> const& uniforms);
    virtual ~ShaderUniformsList();
protected:
    mr::TStaticArray<IShaderUniform*> _uniforms;
};

/**
    For shader compilation, use ShaderCompiler
**/

class IShader : public GPUObjectHandle {
public:
    enum Type {
        None = 0,
        Vertex = 0x8B31,
        Fragment = 0x8B30,
        Compute = 0x91B9,
        TessControl = 0x8E88,
        TessEvaluation = 0x8E87,
        Geometry = 0x8DD9,
        TypesNum = 8
    };

    mu::Event<IShader*, const std::string&, const IShader::Type&> OnCompiled; //as args(new code, new shader type)

    //virtual const unsigned int& GetGPUHandle() = 0; GPUObjectHandle
    virtual IShader::Type GetType() = 0;
    virtual bool IsCompiled() = 0;
    virtual bool Compile(IShader::Type const& type, std::string const& code) = 0;

    virtual ~IShader() {}
};

struct ShaderProgramCache {
public:
    unsigned int format;
    mr::TStaticArray<unsigned char> data;

    ShaderProgramCache& operator = (ShaderProgramCache& c);

    ShaderProgramCache();
    ShaderProgramCache(unsigned int const& f, mr::TStaticArray<unsigned char> d);
};

class IShaderProgram : public GPUObjectHandle, public Usable {
public:
    /** Handle uniforms **/
    virtual IShaderUniform* CreateUniform(const std::string& name, const mr::IShaderUniform::Type& type, void* value) = 0;

    virtual void DeleteUniform(IShaderUniform* su) = 0;
    virtual IShaderUniform* FindShaderUniform(const std::string& name) = 0;

    virtual void SetUniform(const std::string& name, const int& value) = 0;
    virtual void SetUniform(const std::string& name, const float& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec2& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec3& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec4& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::mat4& value) = 0;

    virtual void UpdateUniforms() = 0; //update all attached uniform values

    virtual TStaticArray<IShaderUniform*> GetShaderUniforms() = 0; //get all uniform handles
    virtual TStaticArray<ShaderUniformInfo> GetCompiledUniforms() = 0; //returns used in shaders uniforms. don't forget to free this array manually
    virtual bool IsUniform(std::string const& uniformName) = 0;

    virtual bool Link(TStaticArray<IShader*> shaders) = 0;
    virtual bool IsLinked() = 0;

    virtual ShaderProgramCache GetCache() = 0;

    //virtual unsigned int GetGPUHandle() = 0; GPUObjectHandle

    virtual ~IShaderProgram() {}
};

class ShaderProgramInstance : public IShaderProgram {
public:
    IShaderUniform* CreateUniform(const std::string& name, const mr::IShaderUniform::Type& type, void* value) override { return _program->CreateUniform(name, type, value); }
    void DeleteUniform(IShaderUniform* su) override { _program->DeleteUniform(su); }
    IShaderUniform* FindShaderUniform(std::string const& name) override { return _program->FindShaderUniform(name); }
    void SetUniform(const std::string& name, const int& value) override { _program->SetUniform(name, value); }
    void SetUniform(const std::string& name, const float& value) override { _program->SetUniform(name, value); }
    void SetUniform(const std::string& name, const glm::vec2& value) override { _program->SetUniform(name, value); }
    void SetUniform(const std::string& name, const glm::vec3& value) override { _program->SetUniform(name, value); }
    void SetUniform(const std::string& name, const glm::vec4& value) override { _program->SetUniform(name, value); }
    void SetUniform(const std::string& name, const glm::mat4& value) override { _program->SetUniform(name, value); }
    TStaticArray<ShaderUniformInfo> GetCompiledUniforms() override { return _program->GetCompiledUniforms(); }
    bool IsUniform(std::string const& uniformName) override { return _program->IsUniform(uniformName); }
    bool Link(TStaticArray<IShader*> shaders) override { return _program->Link(shaders); }
    bool IsLinked() override { return _program->IsLinked(); }
    ShaderProgramCache GetCache() override { return _program->GetCache(); }

    TStaticArray<IShaderUniform*> GetShaderUniforms() override { return _program->GetShaderUniforms().Combine(_uniforms.GetList()); }
    void UpdateUniforms() override { _program->UpdateUniforms(); _uniforms.Update(); }

    inline bool Use() override {
        bool b = _program->Use();
        if(b) _uniforms.Update();
        return b;
    }

    ShaderProgramInstance() : _program(nullptr) {}
    ShaderProgramInstance(ShaderUniformsList const& uniforms, IShaderProgram* program) : _uniforms(uniforms), _program(program) {}
    virtual ~ShaderProgramInstance() {}
protected:
    ShaderUniformsList _uniforms;
    IShaderProgram* _program;
};

}

#endif
