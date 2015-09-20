#pragma once

#ifndef _MR_SHADER_INTERFACES_H_
#define _MR_SHADER_INTERFACES_H_

#include "../CoreObjects.hpp"
#include "ShaderTypes.hpp"

#include <mu/Containers.hpp>

#include "../pre_glm.hpp"
#include <functional>

namespace mr {

class IShaderProgram;
class ShaderUniformMap;

class IShaderUniformRef {
    friend class IShaderProgram;
public:
    enum Type : unsigned char {
        Float = 0,
        Vec2 = 1, //glm::vec2 float[2]
        Vec3 = 2, //glm::vec3 float[3]
        Vec4 = 3, //glm::vec4 float[4]
        Mat4 = 4, //glm::mat4 float[4][4]
        Int = 5,  //int
        UInt64 = 6, //uint64_t
        Sampler1D = 7,//int
        Sampler2D = 8,//int
        Sampler3D = 9 //int
    };

    static std::string TypeToString(IShaderUniformRef::Type const& t);

    virtual std::string GetName() = 0;
    virtual IShaderUniformRef::Type GetType() = 0;
    virtual void SetValuePtr(void* ptr)  = 0;
    virtual void* GetValuePtr() = 0;
    virtual int GetGPULocation() = 0;
    virtual void Update() = 0;

    virtual ShaderUniformMap* GetMap() = 0;

    virtual ~IShaderUniformRef() {}
};

class IShader : public IGPUObjectHandle {
public:
    virtual ShaderType GetType() = 0;
    virtual bool IsCompiled() = 0;

    virtual ~IShader() {}
protected:
    virtual bool Create(ShaderType const& type) = 0;
};

struct ShaderProgramCache {
public:
    unsigned int format;
    mu::ArrayHandle<unsigned char> data;

    ShaderProgramCache& operator = (ShaderProgramCache& c);

    ShaderProgramCache();
    ShaderProgramCache(unsigned int const& f, mu::ArrayHandle<unsigned char> d);
};

class IShaderProgram : public IGPUObjectHandle {
public:
    virtual bool IsLinked() = 0;
    virtual ShaderProgramCache GetCache() = 0;
    virtual ShaderUniformMap* GetMap() = 0;

    virtual ~IShaderProgram() {}
protected:
    virtual bool Create() = 0;
};

}

#endif
