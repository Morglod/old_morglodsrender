#pragma once

#ifndef _MR_SCENE_LOADER_H_
#define _MR_SCENE_LOADER_H_

#include "../Utils/Containers.hpp"
#include <string>

namespace mr {

class IMesh;
class IMaterial;
class IGeometry;

class SceneLoader final {
public:
    struct ImportOptions {
        bool fixInfacingNormals = false; //May occur problems.
        bool flipUVs = true;
        bool generateUVs = true;
        bool transformUVs = true;
        bool fast = false; //Not max quality?
        bool debugLog = false;
    };

    bool Import(std::string const& file, ImportOptions const& options);

    unsigned int GetMeshesNum();
    TStaticArray<IMesh*> GetMeshes();

    unsigned int GetGeometriesNum();
    TStaticArray<IGeometry*> GetGeometry();

    unsigned int GetMaterialsNum();
    TStaticArray<IMaterial*> GetMaterials();

    SceneLoader();
    ~SceneLoader();
private:
    class Impl;
    std::shared_ptr<SceneLoader::Impl> _impl;
};

}

#endif // _MR_SCENE_LOADER_H_
