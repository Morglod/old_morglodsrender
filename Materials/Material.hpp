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
    bool IsTwoSided() override { return false; }
    bool IsDebugOnly() override { return false; }

    void OnMaterialFlagChanged(MaterialFlag const& newFlag) override;

    inline mr::TStaticArray<MaterialFlag> GetAvailableFlags() override { return mr::TStaticArray<MaterialFlag> { MaterialFlag_Default }; }

    inline MaterialDescr GetDescription() override { return _descr; }
    inline MaterialShaderParams GetShaderParams() override { return _shader_descr; }

    DefaultMaterial();
    virtual ~DefaultMaterial();
protected:
    IShaderProgram* _program;
    MaterialDescr _descr;
    MaterialShaderParams _shader_descr;
};

MaterialFlag MaterialFlagGetUsed();
void MaterialFlagUse(MaterialFlag const& flag);

IMaterial* MaterialGetUsed();
void MaterialUse(IMaterial* mat);

}

#endif
