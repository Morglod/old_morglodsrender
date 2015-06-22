#pragma once

#include "ModelInterfaces.hpp"

namespace mr {

class SubModel : public ISubModel {
public:
    mr::TStaticArray<MeshPtr> GetMeshes() const override { return _meshes; }
    void SetMeshes(mr::TStaticArray<MeshPtr> newMeshes) override { _meshes = newMeshes; }

    //may be nullptr
    MaterialWeakPtr GetMaterial() const override { return _material; }

    //override all meshes materials
    void SetMaterial(MaterialWeakPtr newMaterial) override { _material = newMaterial; }

    void Draw(glm::mat4* modelMatrix) const override;

    float GetMinDist() const override { return _dist; }
    void SetMinDist(float dist) override { _dist = dist; }

    SubModel();
    virtual ~SubModel();
protected:
    mr::TStaticArray<MeshPtr> _meshes;
    MaterialWeakPtr _material;
    float _dist = 1.0f;
};

class Model : public IModel {
public:
    mr::TStaticArray<SubModelPtr> GetLods() const override { return _lods; }
    void SetLods(mr::TStaticArray<SubModelPtr> newLods) override { _lods = newLods; }

    float GetLodBias() const override { return _bias; }
    void SetLodBias(float const& bias) override { _bias = bias; }

    SubModelPtr GetLod(size_t const& lodIndex) const override { return _lods[lodIndex]; }
    size_t GetLodIndexAtDistance(float dist) const override;
    SubModelPtr GetLodAtDistance(float const& dist) const override { return _lods[GetLodIndexAtDistance(dist)]; }
    bool IsVisibleAtDist(float dist) const override;

    virtual ~Model() {}
protected:
    float _bias = 1.0f;
    mr::TStaticArray<SubModelPtr> _lods;
};

}
