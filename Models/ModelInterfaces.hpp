#pragma once

#ifndef _MR_MODEL_INTERFACES_H_
#define _MR_MODEL_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Scene/Transformation.hpp"

namespace MR {

class IMesh;

/*
    Mesh + Dynamic content (Anim, etc)
*/
class ISubModel {
public:
    virtual MR::TStaticArray<IMesh*> GetMeshes() = 0;
    virtual void SetMeshes(MR::TStaticArray<IMesh*>) = 0;

    virtual IMaterial* GetMaterial() = 0; //may be nullptr
    virtual void SetMaterial(IMaterial*) = 0; //override all meshes materials

    virtual void Draw() = 0;
};

class IModel {
public:
    //LOD
    virtual MR::TStaticArray<ISubModel*> GetLevels() = 0;
    virtual void SetLevels(MR::TStaticArray<ISubModel*>) = 0;

    virtual float GetLodBias() = 0;
    virtual void SetLodBias(float const& bias) = 0;

    virtual ISubModel* GetLod(size_t const& lodIndex) = 0;
    virtual size_t GetLodIndexAtDistance(float const& dist) = 0;
    virtual ISubModel* GetLodAtDistance(float const& dist) = 0;
    //
};

}

#endif
