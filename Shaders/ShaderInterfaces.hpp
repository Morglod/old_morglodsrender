#pragma once

#ifndef _MR_SHADER_INTERFACES_H_
#define _MR_SHADER_INTERFACES_H_

#include "../CoreObjects.hpp"
#include <Containers.hpp>

#include <glm/glm.hpp>
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
        Sampler1D,//int
        Sampler2D,//int
        Sampler3D //int
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

    virtual IShader::Type GetType() = 0;
    virtual bool IsCompiled() = 0;

    virtual ~IShader() {}
protected:
    virtual bool Create(IShader::Type const& type) = 0;
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
