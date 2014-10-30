#pragma once

#ifndef _MR_MATERIAL_H_
#define _MR_MATERIAL_H_

#include "MaterialInterfaces.hpp"

namespace MR {

class IShaderProgram;

class DefaultMaterial : public IMaterial {
public:
    bool Use() override;
    bool IsTwoSided() override { return false; }
    bool IsDebugOnly() override { return false; }

    void OnMaterialFlagChanged(MaterialFlag const& newFlag) override;

    MR::TStaticArray<MaterialFlag> GetAvailableFlags() override {
        return MR::TStaticArray<MaterialFlag> { MaterialFlag::Default() };
    }

    DefaultMaterial();
    virtual ~DefaultMaterial();
protected:
    IShaderProgram* _program;
};

MaterialFlag MaterialFlagGetUsed();
void MaterialFlagUse(MaterialFlag const& flag);

IMaterial* MaterialGetUsed();
void MaterialUse(IMaterial* mat);

}

#endif
