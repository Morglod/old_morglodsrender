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
    MR::Event<Material**, unsigned int> OnMaterialsChanged;
    MR::Event<GeometryBuffer**, unsigned int> OnGeometryBuffersChanged;
    MR::Event<const bool&> OnResourceFreeStateChanged;

    /** If true, resource data will be destroyed after unloading/deleting resource
    *  For example geometry buffers after unloading/deleting Mesh resource
    *  True by default
    */
    virtual void SetResourceFreeState(const bool& state);

    void SetMaterials(Material** m, const unsigned int & mnum);
    void SetGeomBuffers(GeometryBuffer** gb, const unsigned int & n);

    inline unsigned int GetGeomBuffersNum(){ return geom_buffers_num; }
    inline Material** GetMaterials(){ return materials; }
    inline unsigned int GetMaterialsNum(){ return materials_num; }
    inline GeometryBuffer** GetGeomBuffers(){ return geom_buffers; }

    /*inline MR::Transform* GetTransform(){
        return _transform;
    }*/

    Mesh(GeometryBuffer** gb, unsigned int nm, Material** m, unsigned int mnum);
    virtual ~Mesh();

protected:
    GeometryBuffer** geom_buffers = nullptr; //array of pointers to GeomBuffers
    unsigned int geom_buffers_num;

    Material** materials = nullptr; //array of pointers to Materials, num of geom_buffers_num; if null, not used; if (SingleMaterial) so it is size of 1
    unsigned int materials_num;

    bool _res_free_state;
};

}

#endif // _MR_MESH_H_
