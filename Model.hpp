#pragma once

#ifndef _MR_MODEL_H_
#define _MR_MODEL_H_

#include "pre.hpp"
#include "Mesh.hpp"
#include "ResourceManager.hpp"

namespace MR{

class Mesh;
class ModelManager;

class Model : public virtual Resource {
protected:
    MR::Mesh** meshes;
    unsigned short meshes_num;

public:
    inline MR::Mesh** GetMeshes(){
        return meshes;
    }

    inline MR::Mesh* GetMesh(unsigned short i){
        return meshes[i];
    }

    inline unsigned short GetMeshesNum(){
        return meshes_num;
    }

    virtual bool Load();
    virtual void UnLoad();

    Model(ModelManager* manager, std::string name, std::string source);
};

struct ModelFile {
public:
    Mesh** meshes;
    unsigned int meshes_num;

    ModelFile();
    static ModelFile* ImportModelFile(std::string file, bool bindexes, bool log);
};

class ModelManager : public virtual ResourceManager {
public:
    virtual Resource* Create(std::string name, std::string source);

    ModelManager() : ResourceManager() {}
    virtual ~ModelManager() {}

    static ModelManager* Instance() {
        static ModelManager* m = new ModelManager();
        return m;
    }
};

}

#endif
