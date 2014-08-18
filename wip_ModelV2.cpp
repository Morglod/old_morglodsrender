#include "MeshV2.hpp"

MR::Mesh* MR::Model::GetMesh(const std::string& name){
    for(auto it = _meshes.begin(); it != _meshes.end(); ++it){
        if((*it)->GetName() == name) return (*it);
    }
    return nullptr;
}

std::vector<Material*> GetAssociatedMaterials(){
    std::vector<Material*> mats;
    mats.resize(_meshes.size());

    for(auto it = _meshes.begin(); it != _meshes.end(); ++it){
        MR::Material* mat = (*it)->GetMaterial();
        if(mat != nullptr) mats.push_back(mat);
    }

    return mats;
}

MR::Mesh::Model() : Object(), _name(""), _meshes(), _material(nullptr) {
}

MR::Mesh::~Model(){
}
