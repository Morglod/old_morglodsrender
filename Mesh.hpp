#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "pre.hpp"
#include "ResourceManager.hpp"

namespace MR {
class GeometryBuffer;
class Material;
class Transform;

class Mesh {
protected:
    GeometryBuffer** geom_buffers = nullptr; //array of pointers to GeomBuffers
    unsigned int geom_buffers_num;

    Material** materials = nullptr; //array of pointers to Materials, num of geom_buffers_num; if null, not used; if (SingleMaterial) so it is size of 1
    unsigned int materials_num;

public:
    bool _res_free_state;

    MR::Event<Material**, unsigned int> OnMaterialsChanged;
    MR::Event<GeometryBuffer**, unsigned int> OnGeometryBuffersChanged;

    inline void SetMaterials(Material** m, unsigned int mnum){
        if((materials != m) || (materials_num != mnum)) {
            materials = m;
            materials_num = mnum;
            OnMaterialsChanged(this, m, mnum);
        }
    }

    inline GeometryBuffer** GetGeomBuffers(){
        return geom_buffers;
    }

    inline void SetGeomBuffers(GeometryBuffer** gb, unsigned int n){
        if( (geom_buffers != gb) || (geom_buffers_num != n) ){
            geom_buffers = gb;
            geom_buffers_num = n;
            OnGeometryBuffersChanged(this, gb, n);
        }
    }

    inline unsigned int GetGeomBuffersNum(){
        return geom_buffers_num;
    }

    inline Material** GetMaterials(){
        return materials;
    }

    inline unsigned int GetMaterialsNum(){
        return materials_num;
    }

    /*inline MR::Transform* GetTransform(){
        return _transform;
    }*/

    Mesh(GeometryBuffer** gb, unsigned int nm, Material** m, unsigned int mnum);
    virtual ~Mesh();
};

}

#endif // _MR_MESH_H_
