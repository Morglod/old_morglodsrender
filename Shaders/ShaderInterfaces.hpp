#pragma once

#ifndef _MR_SHADER_INTERFACES_H_
#define _MR_SHADER_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Utils/Events.hpp"
#include "../Types.hpp"

#include <string>

namespace MR {

class IShaderProgram;

class IShaderUniform {
    friend class IShaderProgram;
public:
    enum Type {
        SUT_Float = 0,
        SUT_Vec2 = 1, //glm::vec2 float[2]
        SUT_Vec3 = 2, //glm::vec3 float[3]
        SUT_Vec4 = 3, //glm::vec4 float[4]
        SUT_Mat4 = 4, //glm::mat4 float[4][4]
        SUT_Int = 5 //int or sampler2D etc; uniform1i
    };

    MR::EventListener<IShaderUniform*, void*> OnNewValuePtr;
    MR::EventListener<IShaderUniform*, IShaderProgram*, const int&> OnMapped; //as args (shader program, new uniform location)

    virtual const std::string& GetName() = 0;
    virtual const IShaderUniform::Type& GetType() = 0;
    virtual void SetPtr(void* ptr)  = 0;
    virtual void* GetPtr() = 0;
    virtual const int& GetGPULocation() = 0;
    virtual bool Map(IShaderProgram* shader) = 0; //reset gpu location

    virtual ~IShaderUniform() {}
};

/**
    For shader compilation, use ShaderCompiler
**/

class IShader : public HandleObject {
public:
    enum Type {
        ST_Vertex = 0x8B31,
        ST_Fragment = 0x8B30,
        ST_Compute = 0x91B9,
        ST_TessControl = 0x8E88,
        ST_TessEvaluation = 0x8E87,
        ST_Geometry = 0x8DD9
    };

    MR::EventListener<IShader*, const std::string&, const IShader::Type&> OnCompiled; //as args(new code, new shader type)

    virtual const unsigned int& GetGPUHandle() = 0;
    virtual const IShader::Type& GetType() = 0;

    virtual bool Attach(IShaderProgram* program) = 0;
    virtual void Detach(IShaderProgram* program) = 0;
    virtual StaticArray<IShaderProgram*> GetConnectedPrograms() = 0;

    virtual ~IShader() {}
};

class IShaderProgram : public HandleObject {
public:
    class Features {
    public:
        //Options
        bool colorFilter = false;
        bool opacityDiscardOnAlpha = true;
        float opacityDiscardValue = 0.9f;

        //List of used maps
        bool ambient = false;
        bool diffuse = false;
        bool displacement = false;
        bool emissive = false;
        bool height = false;
        bool baked_lightmap = false;
        bool normal = false;
        bool opacity = false;
        bool reflection = false;
        bool shininess = false;
        bool specular = false;

        //Output (not both, use only one of two if needed)
        bool toRenderTarget = false;
        bool toScreen = false;

        //Tech
        bool defferedRendering = false; //if toRenderTarget is false, lights and effects will be calculated from buffers, otherwise, only buffers will be writed
        bool light = true;
        bool fog = false;

        bool operator == (const IShaderProgram::Features& dsr1) const;
    };

    virtual bool BindDefaultShaderInOut() = 0; /// Call before linking

    virtual IShaderUniform* CreateUniform(const std::string& name, const MR::IShaderUniform::Type& type, void* value) = 0;

    virtual void SetUniform(const std::string& name, const int& value) = 0;
    virtual void SetUniform(const std::string& name, const float& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec2& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec3& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::vec4& value) = 0;
    virtual void SetUniform(const std::string& name, const glm::mat4& value) = 0;

    virtual size_t GetShaderUniformsPtr(IShaderUniform*** list_ptr) = 0; //out as (IShaderUniform** ptr) &ptr

    virtual void DeleteUniform(IShaderUniform* su) = 0;
    virtual IShaderUniform* FindShaderUniform(const std::string& name) = 0;

    virtual const unsigned int& GetGPUHandle() = 0;
    virtual const Features& GetFeatures() = 0;

    virtual ~IShaderProgram() {}
};

}

#endif
