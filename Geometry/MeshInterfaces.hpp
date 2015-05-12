#pragma once

#ifndef _MR_MESH_INTERFACES_H_
#define _MR_MESH_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include <mu/Events.hpp>

#include <glm/glm.hpp>

namespace mr {

class IGeometry;
class IMaterial;

/*
    Only static geometry + material
*/
class IMesh {
public:
    mu::Event<IMesh*, IMaterial*> OnMaterialChanged;
    mu::Event<IMesh*, const TStaticArray<IGeometry*>&> OnGeometryChanged;

    virtual void SetMaterial(IMaterial* m) = 0;
    virtual IMaterial* GetMaterial() = 0;

    virtual void SetGeometry(TStaticArray<IGeometry*>& geom) = 0;
    virtual TStaticArray<IGeometry*> GetGeometry() = 0;

    virtual void Draw(glm::mat4* modelMat) = 0;

    virtual ~IMesh() {}
};

typedef std::shared_ptr<IMesh> MeshPtr;
typedef std::weak_ptr<IMesh> MeshWeakPtr;

}

#endif
