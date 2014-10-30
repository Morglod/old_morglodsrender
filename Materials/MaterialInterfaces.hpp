#pragma once

#ifndef _MR_MATERIAL_INTERFACES_H_
#define _MR_MATERIAL_INTERFACES_H_

#include "../Utils/Containers.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

struct MaterialFlag {
public:
    unsigned char flag; //if always==false, use this pass only if this flag is On
    MaterialFlag() : flag(0) {}
    MaterialFlag(unsigned char Flag) : flag(Flag) {}

    inline static MaterialFlag Default(){return MaterialFlag(0);} //default rendering state
    inline static MaterialFlag ShadowMap(){return MaterialFlag(1);} //drawing to shadow map
    inline static MaterialFlag ToTexture(){return MaterialFlag(2);} //drawing to render target
    inline static MaterialFlag Postprocess() {return MaterialFlag(3);}
    inline bool operator == (const MaterialFlag& flag_) {
        return (flag == flag_.flag);
    }
};

class IMaterial {
public:
    virtual bool Use() = 0;
    virtual bool IsTwoSided() = 0;
    virtual bool IsDebugOnly() = 0;

    virtual void OnMaterialFlagChanged(MaterialFlag const& newFlag) = 0;

    virtual MR::TStaticArray<MaterialFlag> GetAvailableFlags() = 0;

    virtual ~IMaterial() {}
};

}

#endif // _MR_MATERIAL_INTERFACES_H_
