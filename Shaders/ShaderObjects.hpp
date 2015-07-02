#pragma once

#ifndef _MR_SHADER_OBJECTS_H_
#define _MR_SHADER_OBJECTS_H_

#include "ShaderInterfaces.hpp"

namespace mr {

class Shader : public IShader {
    friend class ShaderManager;
public:
    inline ShaderType GetType() override { return _type; }
    inline bool IsCompiled() override { return _compiled; }

    //ObjectHandle
    void Destroy() override;

    virtual ~Shader();
protected:
    bool Create(ShaderType const& type) override;
    Shader();
    ShaderType _type;
    bool _compiled;
};

}

#endif
