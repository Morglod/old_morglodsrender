#include "Mesh.hpp"
#include "../Utils/Log.hpp"
#include "../Materials/MaterialInterfaces.hpp"
#include "GeometryInterfaces.hpp"
#include "../Utils/Debug.hpp"

void MR::Mesh::Draw() {
    _mat->Use();
    for(size_t i = 0; i < _geom.GetNum(); ++i){
        _geom.GetRaw()[i]->Draw();
    }
}

MR::Mesh::Mesh(const TStaticArray<IGeometry*>& geom, IMaterial* mat)
 : _geom(geom), _mat(mat) {
}

MR::Mesh::~Mesh() {
}

MR::IMesh* MR::Mesh::Create(const TStaticArray<IGeometry*>& geom, IMaterial* mat) {
    return dynamic_cast<MR::IMesh*>(new MR::Mesh(geom, mat));
}
