#pragma once

#ifndef _MR_MESH_H_
#define _MR_MESH_H_

#include "MorglodsRender.hpp"

namespace MR {
class GeometryBuffer;
class Material;
class MeshManager;
class Transform;

class Mesh : public virtual Resource {
protected:
    MR::Transform* _transform = nullptr;

    GeometryBuffer** geom_buffers = nullptr; //array of pointers to GeomBuffers
    unsigned int geom_buffers_num;

    Material** materials = nullptr; //array of pointers to Materials, num of geom_buffers_num; if null, not used; if (SingleMaterial) so it is size of 1
    unsigned int materials_num;

public:
    inline GeometryBuffer** GetGeomBuffers(){
        return geom_buffers;
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

    inline MR::Transform* GetTransform(){
        return _transform;
    }

    virtual bool Load();
    virtual void UnLoad();

    Mesh(MeshManager* manager, std::string name, std::string source);
    virtual ~Mesh();
};

class MeshManager : public virtual ResourceManager {
public:
    virtual Resource* Create(std::string name, std::string source);

    MeshManager() : ResourceManager() {}
    virtual ~MeshManager() {}

    static MeshManager* Instance() {
        static MeshManager* m = new MeshManager();
        return m;
    }
};
}

#endif // _MR_MESH_H_
