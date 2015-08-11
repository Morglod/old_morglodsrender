#pragma once

#include "ShaderInterfaces.hpp"
#include "ShaderUniforms.hpp"

namespace mr {

class ShaderProgram : public IShaderProgram {
    friend class ShaderManager;
public:
    inline ShaderUniformMap* GetMap() override { return &_uniformMap; }
    inline bool IsLinked() override { return _linked; }

    ShaderProgramCache GetCache() override;

    //ObjectHandle
    void Destroy() override;

    virtual ~ShaderProgram();
protected:
    bool Create() override;

    ShaderProgram();
    bool _linked;
    ShaderUniformMap _uniformMap;
};

}

