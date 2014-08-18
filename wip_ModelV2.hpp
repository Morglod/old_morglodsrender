#pragma once

#ifndef _MR_MODEL_H_
#define _MR_MODEL_H_

#include "Types.hpp"
#include <string>
#include <vector>

namespace MR {

class Mesh;
class Material;

class Model : public Object {
public:
    inline std::string GetName() { return _name; }
    inline Mesh** GetMeshes() { return _meshes.data(); }
    inline size_t GetMeshesNum() { return _meshes.size(); }
    inline Mesh* GetMesh(const size_t& i) { return _meshes[i]; }
    Mesh* GetMesh(const std::string& name);

    inline Material* GetMaterial() { return _material; } //material that redefines using of mesh's material
    std::vector<Material*> GetAssociatedMaterials();

    inline std::string ToString() override { return "Model("+_name+")"; }

    Model();
    ~Model();
protected:
    std::string _name;
    std::vector<Mesh*> _meshes;
    Material* _material;
};

}

#endif // _MR_MODEL_H_
