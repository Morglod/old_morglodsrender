#pragma once

#ifndef _MR_SHADER_UNIFORMS_H_
#define _MR_SHADER_UNIFORMS_H_

#include "ShaderInterfaces.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace mr {

/** Named uniform of shader
 *  After shader recompilation, call ShaderUniform::ResetLocation method
 */
class ShaderUniform : public IShaderUniform {
public:
    inline std::string GetName() override { return _name; }
    inline IShaderUniform::Type GetType() override { return _type; }
    inline void SetPtr(void* ptr) override { _value = ptr; OnNewValuePtr(dynamic_cast<IShaderUniform*>(this), ptr); }
    inline void* GetPtr() override { return _value; }
    inline int GetGPULocation() override { return _gpu_location; }
    void Update() override;

    bool ResetLocation() override;

    inline IShaderProgram* GetParent() override { return _owner; }

    //ResetLocation will be called
    ShaderUniform();
    ShaderUniform(const std::string& name, const IShaderUniform::Type& type, void* value, IShaderProgram* program);
    virtual ~ShaderUniform() {}
protected:
    std::string _name;
    int _gpu_location;
    IShaderUniform::Type _type;
    void* _value;
    IShaderProgram* _owner;
};

}

#endif
