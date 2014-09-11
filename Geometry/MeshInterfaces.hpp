#pragma once

#ifndef _MR_MESH_INTERFACES_H_
#define _MR_MESH_INTERFACES_H_

#include "../Utils/Containers.hpp"
#include "../Utils/Events.hpp"

namespace MR {

class IGeometry;
class IMaterial;

class IMesh {
public:
    MR::EventListener<IMesh*, IMaterial*> OnMaterialChanged;
    MR::EventListener<IMesh*, const TStaticArray<IGeometry*>&> OnGeometryChanged;

    virtual void SetMaterial(IMaterial* m) = 0;
    virtual IMaterial* GetMaterial() = 0;

    virtual void SetGeometry(TStaticArray<IGeometry*>& geom) = 0;
    virtual TStaticArray<IGeometry*> GetGeometry() = 0;

    virtual void Draw() = 0;

    virtual ~IMesh() {}
};

}

#endif
