#pragma once

#include "ModelInterfaces.hpp"

namespace mr {

class SubModel : public ISubModel {
public:
    mr::TStaticArray<MeshWeakPtr> GetMeshes() const override { return _Meshes; }
    void SetMeshes(mr::TStaticArray<MeshWeakPtr> newMeshes) override { _meshes = newMeshes; }

    //may be nullptr
    MaterialWeakPtr GetMaterial() const override { return _material; }

    //override all meshes materials
    void SetMaterial(MaterialWeakPtr newMaterial) override { _material = newMaterial; }

    void Draw(glm::mat4* modelMatrix) const override;

    virtual ~SubModel() {}
protected:
    mr::TStaticArray<MeshWeakPtr> _meshes;
    MaterialWeakPtr _material;
};

class Model : public IModel {
public:
    mr::TStaticArray<SubModelPtr> GetLevels() override;
    void SetLevels(mr::TStaticArray<SubModelPtr>) override;

    float GetLodBias() override;
    void SetLodBias(float const& bias) override;

    SubModelPtr GetLod(size_t const& lodIndex) override;
    size_t GetLodIndexAtDistance(float const& dist) override;
    SubModelPtr GetLodAtDistance(float const& dist) override;

    virtual ~Model() {}
};

}
