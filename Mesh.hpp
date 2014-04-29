#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "ResourceManager.hpp"

namespace MR {
class IGeometry;
class Material;
class Transform;

class Mesh {
public:
    MR::EventListener<Mesh*, Material*> OnMaterialChanged;
    MR::EventListener<Mesh*, IGeometry**, unsigned int> OnGeometryChanged;
    MR::EventListener<Mesh*, const bool&> OnResourceFreeStateChanged;

    /** If true, resource data will be destroyed after unloading/deleting resource
    *  For example geometry buffers after unloading/deleting Mesh resource
    *  True by default
    */
    virtual void SetResourceFreeState(const bool& state);

    void SetMaterial(Material* m);
    void SetGeoms(IGeometry** gb, const unsigned int & n);

    inline IGeometry** GetGeoms(){ return geoms; }
    inline unsigned int GetGeomNum(){ return geoms_num; }

    inline Material* GetMaterial(){ return material; }

    /*  gb - array of pointers to Geometry
    */
    Mesh(IGeometry** gb, unsigned int nm, Material* m);
    virtual ~Mesh();

protected:
    IGeometry** geoms = nullptr; //array of pointers to Geometry
    unsigned int geoms_num;

    Material* material = nullptr;

    bool _res_free_state;
};

}

#endif // _MR_MESH_H_
