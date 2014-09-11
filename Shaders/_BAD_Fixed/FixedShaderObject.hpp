#pragma once

#ifndef _MR_FIXED_SHADER_OBJECT_H_
#define _MR_FIXED_SHADER_OBJECT_H_

#include "FixedShaderInterfaces.hpp"

namespace MR {

class FixedShaderProgram : public IFixedShaderProgram {
public:
    void SetTexture(TextureMaps const& texMap, ITexture* tex) override;
    inline ITexture* GetTexture(TextureMaps const& texMap) override { return _maps[(size_t)texMap]; }

    void SetDisplacementTech(DisplacementTech const& tech) override;
    inline DisplacementTech GetDisplacementTech() override { return _disp; }

    bool Use() override;

    FixedShaderProgram();
    virtual ~FixedShaderProgram();
protected:
    ITexture* _maps[TextureMapsNum];
    DisplacementTech _disp;
};

}

#endif
