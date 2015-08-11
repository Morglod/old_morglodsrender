#pragma once

#ifndef _MR_SCENE_LOADER_H_
#define _MR_SCENE_LOADER_H_

#include "../Utils/Containers.hpp"
#include <string>

namespace mr {

class IMesh;
class IMaterial;
class IGeometry;
class Texture2D;

class IModel;
typedef std::shared_ptr<IModel> ModelPtr;

class TextureData;
typedef std::shared_ptr<TextureData> TextureDataPtr;

class SceneLoader final {
public:
    struct ProgressInfo {
        unsigned int    meshes, totalMeshes,
                        materials, totalMaterials;
    };

    struct ImportOptions {
        bool fixInfacingNormals = false; //May occur problems.
        bool flipUVs = true;
        bool generateUVs = true;
        bool transformUVs = true;
        bool fast = false; //Not max quality?
        bool debugLog = false;
        std::function<bool (float percentage)> assimpProcessCallback = nullptr;
        std::function<void (ProgressInfo const& info)> progressCallback = nullptr;
        std::function<bool (Texture2D* texture, TextureDataPtr const& data)> customTextureLoad = nullptr; //it should load and call Complete function on already allocated texture
    };

    bool Import(std::string const& file, ImportOptions const& options);

    unsigned int GetMeshesNum();
    TStaticArray<IMesh*> GetMeshes();

    unsigned int GetGeometriesNum();
    TStaticArray<IGeometry*> GetGeometry();

    unsigned int GetMaterialsNum();
    TStaticArray<IMaterial*> GetMaterials();

    ModelPtr GetModel();

    SceneLoader();
    ~SceneLoader();
private:
    class Impl;
    std::shared_ptr<SceneLoader::Impl> _impl;
};

}

#endif // _MR_SCENE_LOADER_H_
