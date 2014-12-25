#pragma once

#ifndef _MR_MODEL_INTERFACES_H_
#define _MR_MODEL_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Scene/Transformation.hpp"

namespace mr {


class IMaterial;
typedef std::weak_ptr<IMaterial> MaterialWeakPtr;

class IMesh;
typedef std::weak_ptr<IMesh> MeshWeakPtr;

/*
    Mesh + Dynamic content (Anim, etc)
*/
class ISubModel {
public:
    virtual mr::TStaticArray<MeshWeakPtr> GetMeshes() const = 0;
    virtual void SetMeshes(mr::TStaticArray<MeshWeakPtr>) = 0;

    virtual MaterialWeakPtr GetMaterial() const = 0; //may be nullptr
    virtual void SetMaterial(MaterialWeakPtr) = 0; //override all meshes materials

    virtual void Draw(glm::mat4* modelMatrix) const = 0;

    virtual ~ISubModel() {}
};

typedef std::shared_ptr<ISubModel> SubModelPtr;

class IModel {
public:
    //LOD
    virtual mr::TStaticArray<ISubModel*> GetLevels() = 0;
    virtual void SetLevels(mr::TStaticArray<ISubModel*>) = 0;

    virtual float GetLodBias() = 0;
    virtual void SetLodBias(float const& bias) = 0;

    virtual ISubModel* GetLod(size_t const& lodIndex) = 0;
    virtual size_t GetLodIndexAtDistance(float const& dist) = 0;
    virtual ISubModel* GetLodAtDistance(float const& dist) = 0;
    //

    virtual ~IModel() {}
};

typedef std::weak_ptr<IModel> ModelWeakPtr;

}

#endif
