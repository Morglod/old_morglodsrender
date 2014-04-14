#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "ResourceManager.hpp"

namespace MR {
class GeometryBuffer;
class Material;
class Transform;

class Mesh {
public:
    MR::Event<Material*> OnMaterialChanged;
    MR::Event<GeometryBuffer**, unsigned int> OnGeometryBuffersChanged;
    MR::Event<const bool&> OnResourceFreeStateChanged;

    /** If true, resource data will be destroyed after unloading/deleting resource
    *  For example geometry buffers after unloading/deleting Mesh resource
    *  True by default
    */
    virtual void SetResourceFreeState(const bool& state);

    void SetMaterial(Material* m);
    void SetGeomBuffers(GeometryBuffer** gb, const unsigned int & n);

    inline GeometryBuffer** GetGeomBuffers(){ return geom_buffers; }
    inline unsigned int GetGeomBuffersNum(){ return geom_buffers_num; }

    inline Material* GetMaterial(){ return material; }

    /*  gb - array of pointers to GeomBuffers
    */
    Mesh(GeometryBuffer** gb, unsigned int nm, Material* m);
    virtual ~Mesh();

protected:
    GeometryBuffer** geom_buffers = nullptr; //array of pointers to GeomBuffers
    unsigned int geom_buffers_num;

    Material* material = nullptr;

    bool _res_free_state;
};

}

#endif // _MR_MESH_H_
