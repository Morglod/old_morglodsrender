#pragma once

#ifndef _MR_MATERIAL_INTERFACES_H_
#define _MR_MATERIAL_INTERFACES_H_

#include "../Utils/Containers.hpp"

#include <string>

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

typedef unsigned int MaterialFlag;

struct MaterialParams {
    glm::vec3 colorAmbient;
    glm::vec3 colorDiffuse; // colorDiffuse + colorAmbient

    float reflectivity; // reflectivity * texReflectivity
    float roughness;
    float emissive;

    enum SubdivType {
        SubdivNone = 0,
        SubdivFlat = 1,
        SubdivPN
    };

    SubdivType subdivType;
    float subdivFactor;

    enum HeightType {
        None = 0,
        Bump = 1,
        Parallax = 2,
        Displacement
    };

    HeightType heightType;
    float heightScale;

    std::string texColor;
    std::string texHeight;
    std::string texReflectivion;
    std::string texEmissiveMask; //R channel
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
