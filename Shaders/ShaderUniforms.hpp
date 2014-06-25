#pragma once

#ifndef _MR_SHADER_UNIFORMS_H_
#define _MR_SHADER_UNIFORMS_H_

#include "ShaderInterfaces.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

/** Named uniform of shader
 *  After shader recompilation, call MapUniform method
 */
class ShaderUniform : public IShaderUniform {
public:
    inline const std::string& GetName() override { return _name; }
    inline const IShaderUniform::Type& GetType() override { return _type; }
    inline void SetPtr(void* ptr) override { _value = ptr; OnNewValuePtr(dynamic_cast<IShaderUniform*>(this), ptr); }
    inline void* GetPtr() override { return _value; }
    inline const int& GetGPULocation() override { return _gpu_location; }

    bool Map(IShaderProgram* program) override;

    //Map will be called
    ShaderUniform(const std::string& name, const ShaderUniform::Type& type, void* value, IShaderProgram* program);
    virtual ~ShaderUniform() {}
protected:
    std::string _name;
    int _gpu_location;
    IShaderUniform::Type _type;
    void* _value;
};

}

#endif
