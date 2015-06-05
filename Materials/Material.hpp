#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "MaterialInterfaces.hpp"

namespace mr {

class IShaderProgram;

class DefaultMaterial : public IMaterial {
public:
    void Create(MaterialDescr const& descr) override;

    bool Use() override;
    inline bool IsTwoSided() override { return _twoSided; }
    inline bool IsDebugOnly() override { return false; }

    virtual void SetTwoSided(bool s) { _twoSided = s; }

    void OnMaterialFlagChanged(MaterialFlag const& newFlag) override;

    inline mr::TStaticArray<MaterialFlag> GetAvailableFlags() override { return mr::TStaticArray<MaterialFlag> { MaterialFlag_Default }; }

    inline MaterialDescr GetDescription() override { return _descr; }
    inline MaterialShaderParams GetShaderParams() override { return _shaderParams; }
    inline IShaderProgram* GetShaderProgram() const override { return _program; }
    inline void SetShaderProgram(IShaderProgram* sp) override { _program = sp; }

    DefaultMaterial();
    virtual ~DefaultMaterial();
protected:
    IShaderProgram* _program;
    MaterialDescr _descr;
    MaterialShaderParams _shaderParams;
    bool _twoSided;
	TextureBindlessBindable colorTexture;
	TextureBindlessBindable normalTexture;
	TextureBindlessBindable specularTexture;
};

MaterialFlag MaterialFlagGetUsed();
void MaterialFlagUse(MaterialFlag const& flag);

IMaterial* MaterialGetUsed();
void MaterialUse(IMaterial* mat);

}

#endif
