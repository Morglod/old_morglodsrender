#pragma once

#ifndef _MR_FIXED_SHADER_INTERFACE_H_
#define _MR_FIXED_SHADER_INTERFACE_H_

#include "../ShaderInterfaces.hpp"

namespace MR {

class ITexture;

class IFixedShaderProgram : public IShaderProgram {
public:


    enum DisplacementTech {
        BumpTech = 0,
        ParallaxTech = 1,
        TesselationTech = 2,
        TesselationParallaxTech = 3,
        DisplacementTechNum
    };

    virtual void SetTexture(TextureMaps const& texMap, ITexture* tex) = 0;
    virtual ITexture* GetTexture(TextureMaps const& texMap) = 0;

    virtual void SetDisplacementTech(DisplacementTech const& tech) = 0;
    virtual DisplacementTech GetDisplacementTech() = 0;

    //bool Use()

    virtual ~IFixedShaderProgram() {}
};

}

#endif
