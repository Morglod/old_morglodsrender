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
    bool Import(std::string const& file, bool fast = false);

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
