#pragma once

#ifndef _MR_SHADER_INTERFACES_H_
#define _MR_SHADER_INTERFACES_H_

#include "../CoreObjects.hpp"

namespace MR {

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

    MR::EventListener<IShaderUniform*, void*> OnNewValuePtr;
    MR::EventListener<IShaderUniform*, IShaderProgram*, const int&> OnNewLocation; //as args (shader program, new uniform location)

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

    MR::EventListener<IShader*, const std::string&, const IShader::Type&> OnCompiled; //as args(new code, new shader type)

    //virtual const unsigned int& GetGPUHandle() = 0; GPUObjectHandle
    virtual IShader::Type GetType() = 0;
    virtual bool IsCompiled() = 0;
    virtual bool Compile(IShader::Type const& type, std::string const& code) = 0;

    virtual ~IShader() {}
};

struct ShaderProgramCache {
public:
    unsigned int format;
    MR::TStaticArray<unsigned char> data;

    ShaderProgramCache& operator = (ShaderProgramCache& c);

    ShaderProgramCache();
    ShaderProgramCache(unsigned int const& f, MR::TStaticArray<unsigned char> d);
};

class IShaderProgram : public GPUObjectHandle, public Usable {
public:
    /** Handle uniforms **/
    virtual IShaderUniform* CreateUniform(const std::string& name, const MR::IShaderUniform::Type& type, void* value) = 0;

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

}

#endif
