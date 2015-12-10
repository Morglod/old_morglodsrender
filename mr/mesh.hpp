#pragma once

#include "mr/build.hpp"
#include "mr/primitive.hpp"
#include "mr/material/material.hpp"

namespace mr {

typedef std::shared_ptr<class Mesh> MeshPtr;

class MR_API Mesh final {
public:
    struct ImportOptions {
        bool fixInfacingNormals = false; //May occur problems.
        bool flipUVs = true;
        bool generateUVs = true;
        bool transformUVs = true;
        bool fast = false; //Not max quality?
        bool debugLog = false;
     };

    virtual bool Draw(uint32_t instancesNum = 1);

    static MeshPtr Create(PrimitivePtr const& prim, MaterialPtr const& material);
    static bool Import(std::string const& file, ShaderProgramPtr const& shaderProgram, std::vector<MeshPtr>& out_meshes, ImportOptions options);
private:
    Mesh() = default;
    PrimitivePtr _primitive;
    MaterialPtr _material;
};

}
