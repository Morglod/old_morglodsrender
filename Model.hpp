#pragma once

#ifndef _MR_MODEL_H_
#define _MR_MODEL_H_

#include "Mesh.hpp"
#include "ResourceManager.hpp"
#include "Events.hpp"

namespace MR{

class Mesh;
class ModelManager;

class ModelLod {
public:
    inline MR::Mesh** GetMeshes(){ return meshes; }
    inline MR::Mesh* GetMesh(const unsigned short & i){ return meshes[i]; }
    inline unsigned short GetMeshesNum(){ return meshes_num; }
    inline void SetMeshes(MR::Mesh** mm, const unsigned short & mnum){ meshes = mm; meshes_num = mnum; }

    ModelLod(){}
    ModelLod(MR::Mesh** mm, const unsigned short & mmnum);
    virtual ~ModelLod();

protected:
    MR::Mesh** meshes; //array of pointers
    unsigned short meshes_num; //size of array
};

class Model : public virtual Resource {
public:
    inline void AddLod(ModelLod* l) { lods.push_back(l); }
    inline void SetDistStep(const float & dist){ dist_step = dist; }

    ModelLod* GetLod(const float & dist);

    inline ModelLod* GetLodN(const unsigned short & i){ return lods[i]; }

    virtual bool Load();
    virtual void UnLoad();

    Model(ModelManager* manager, std::string name, std::string source);
    virtual ~Model();

protected:
    /** from 0 dist to dist_step is 0 lod, from dist_step to 2dist_step is 1 lod, etc **/
    std::vector<ModelLod*> lods;
    float dist_step = 7.0f;
};

struct ModelFile {
public:
    Mesh** meshes;
    unsigned short meshes_num;
    ModelFile();
    static ModelFile* ImportModelFile(std::string file, bool bindexes, bool log);
};

class ModelManager : public virtual ResourceManager {
public:
    virtual Resource* Create(const std::string& name, const std::string& source);
    inline Model* NeedModel(const std::string& source) { return dynamic_cast<Model*>(Need(source)); }

    ModelManager() : ResourceManager() {}
    virtual ~ModelManager() {}
    static ModelManager* Instance() {
        static ModelManager* m = new ModelManager();
        return m;
    }
};

}

#endif
