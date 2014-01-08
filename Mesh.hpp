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
    std::string _name = "";
    std::string _source = "";
    bool _loaded = false;
    MeshManager* _manager = nullptr;
    MR::Transform* _transform = nullptr;
    bool _res_free_state = true;

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

    //RESOURCE
    inline void SetResourceFreeState(bool s){
        _res_free_state = s;
    }

    inline bool GetResourceFreeState(){
        return _res_free_state;
    }

    virtual inline std::string GetName() {
        return this->_name;
    }
    virtual inline std::string GetSource() {
        return this->_source;
    }
    virtual bool Load();
    virtual void UnLoad();
    virtual bool ReLoad();
    virtual bool IsLoaded() {
        return this->_loaded;
    }
    inline ResourceManager* GetManager() {
        return (ResourceManager*)this->_manager;
    }
    //---------

    Mesh(MeshManager* manager, std::string name, std::string source);
    virtual ~Mesh();
};

class MeshManager : public virtual ResourceManager {
protected:
    std::vector<Mesh*> _resources;
public:
    bool debugMessages = false;

    inline void Add(Resource* res) {
        this->_resources.push_back( dynamic_cast<Mesh*>(res) );
    }

    inline void Remove(Resource* res) {
        auto it = std::find(this->_resources.begin(), this->_resources.end(), res);
        if(it != this->_resources.end()) {
            delete (*it);
            this->_resources.erase(it);
        }
    }
    virtual Resource* Create(std::string name, std::string source);
    virtual Resource* CreateAndLoad(std::string name, std::string source);
    virtual Resource* Get(std::string source);
    virtual Resource* Find(std::string name);

    virtual Resource* Need(std::string source);

    inline virtual Resource** Need(std::string* source, unsigned int sources_num){
        return nullptr;
    }

    virtual void Remove(std::string name, std::string source);

    virtual void RemoveAll();

    MeshManager() {}
    virtual ~MeshManager() {}

    static MeshManager* Instance() {
        static MeshManager* m = new MeshManager();
        return m;
    }
};
}

#endif // _MR_MESH_H_
