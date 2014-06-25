#pragma once

#ifndef _MR_MODEL_H_
#define _MR_MODEL_H_

#include "Mesh.hpp"
#include "ResourceManager.hpp"
#include "Utils/Events.hpp"
#include "Boxes.hpp"

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
    friend class ModelManager;
public:
    inline void AddLod(ModelLod* l) { lods.push_back(l); }
    inline void SetDistStep(const float & dist){ dist_step = dist; }
    inline Box* GetAABBP() { return &_aabb; }

    ModelLod* GetLod(const float & dist);
    inline ModelLod* GetLodN(const unsigned short & i){ if(GetLodNum() == 0) return nullptr; return lods[i]; }
    inline unsigned short GetLodNum(){ return lods.size(); }

    Model(ResourceManager* manager, std::string name, std::string source);
    virtual ~Model();

protected:
    bool _Loading() override;
    void _UnLoading() override;

    /** from 0 dist to dist_step is 0 lod, from dist_step to 2dist_step is 1 lod, etc **/
    std::vector<ModelLod*> lods;
    float dist_step = 7.0f;
    Box _aabb;

    bool _async_loading = MR_ASYNC_LOADING_DEFAULT;
    AsyncHandle _async_handle; //return value is 1 or 0 (true/false)
};

struct ModelFile {
public:
    Mesh** meshes;
    unsigned short meshes_num;
    glm::vec3 minPoint, maxPoint;

    ModelFile();

    class ImportSettings {
    public:
        bool indecies;
        bool combineSameMaterialsGeom;
        bool ambientTextures;
        ImportSettings() : indecies(true), combineSameMaterialsGeom(true), ambientTextures(false) {}
    };

    static ModelFile* ImportModelFile(std::string file, bool log, const ImportSettings& settings = ImportSettings());
};

class ModelManager : public virtual ResourceManager {
public:
    virtual Resource* Create(const std::string& name, const std::string& source);
    inline Model* NeedModel(const std::string& source) { return dynamic_cast<Model*>(Need(source)); }

    ModelManager() : ResourceManager() {}
    virtual ~ModelManager() {}

    static ModelManager* Instance();
    static void DestroyInstance();
};

}

#endif
