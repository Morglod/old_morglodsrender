#pragma once

#ifndef _MR_MATERIAL_INTERFACES_H_
#define _MR_MATERIAL_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Textures/Texture.hpp"

#include <string>

#include "../pre_glm.hpp"

namespace mr {

class IShaderProgram;

typedef unsigned int MaterialFlag;
const MaterialFlag MaterialFlag_Default = 0;

struct MaterialDescr {
	enum SubdivType {
		SubdivNone = 0,
		SubdivFlat = 1,
		SubdivPN
	};

	enum HeightType {
		None = 0,
		Bump = 1,
		Parallax = 2,
		Displacement
	};

	glm::vec4 colorAmbient; //rgba
	glm::vec3 colorDiffuse;

	TextureBindlessBindable colorTexture;
	TextureBindlessBindable normalTexture;
	TextureBindlessBindable specularTexture;
};

struct MaterialShaderParams {
	struct TextureHandle {
		unsigned int handle;
    };

    glm::vec4 colorAmbient; //rgba
    glm::vec3 colorDiffuse; // colorDiffuse + colorAmbient

    TextureHandle texColor;

    /*float reflectivity; // reflectivity * texReflectivity
    float roughness;
    float emissive;

    char subdivType;
    float subdivFactor;

    char heightType;
    float heightScale;*/
};

class IMaterial {
public:
    virtual void Create(MaterialDescr const& descr) = 0;

    virtual bool Use() = 0;
    virtual bool IsTwoSided() = 0;
    virtual bool IsDebugOnly() = 0;

    virtual void OnMaterialFlagChanged(MaterialFlag const& newFlag) = 0;

    virtual mr::TStaticArray<MaterialFlag> GetAvailableFlags() = 0;

    virtual MaterialShaderParams GetShaderParams() = 0;
    virtual MaterialDescr GetDescription() = 0;
    virtual IShaderProgram* GetShaderProgram() const = 0;
    virtual void SetShaderProgram(IShaderProgram* sp) = 0;

    virtual ~IMaterial() {}
};

typedef std::weak_ptr<IMaterial> MaterialWeakPtr;

}

#endif // _MR_MATERIAL_INTERFACES_H_
