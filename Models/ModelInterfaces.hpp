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

    virtual float GetMinDist() const = 0;
    virtual void SetMinDist(float dist) = 0;

    virtual ~ISubModel() {  }
};

typedef std::shared_ptr<ISubModel> SubModelPtr;

class IModel {
public:
    virtual mr::TStaticArray<SubModelPtr> GetLods() const = 0;
    virtual void SetLods(mr::TStaticArray<SubModelPtr>) = 0;

    virtual float GetLodBias() const = 0;
    virtual void SetLodBias(float const& bias) = 0;

    virtual SubModelPtr GetLod(size_t const& lodIndex) const = 0;
    virtual size_t GetLodIndexAtDistance(float dist) const = 0;
    virtual SubModelPtr GetLodAtDistance(float const& dist) const = 0;

    //Is any lod visible at this dist.
    virtual bool IsVisibleAtDist(float dist) const = 0;

    virtual ~IModel() {}
};

typedef std::shared_ptr<IModel> ModelPtr;
typedef std::weak_ptr<IModel> ModelWeakPtr;

}

#endif
