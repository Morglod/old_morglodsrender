#pragma once

#ifndef _MR_MESH_INTERFACES_H_
#define _MR_MESH_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Utils/Events.hpp"

namespace mr {

class IGeometry;
class IMaterial;

/*
    Only static geometry + material
*/
class IMesh {
public:
    mr::EventListener<IMesh*, IMaterial*> OnMaterialChanged;
    mr::EventListener<IMesh*, const TStaticArray<IGeometry*>&> OnGeometryChanged;

    virtual void SetMaterial(IMaterial* m) = 0;
    virtual IMaterial* GetMaterial() = 0;

    virtual void SetGeometry(TStaticArray<IGeometry*>& geom) = 0;
    virtual TStaticArray<IGeometry*> GetGeometry() = 0;

    virtual void Draw() = 0;

    virtual ~IMesh() {}
};

}

#endif
