#include "Mesh.hpp"
#include "../Utils/Log.hpp"
#include "../Materials/MaterialInterfaces.hpp"
#include "GeometryInterfaces.hpp"
#include "../Utils/Debug.hpp"

void mr::Mesh::Draw() {
    _mat->Use();
    for(size_t i = 0; i < _geom.GetNum(); ++i){
        _geom.GetRaw()[i]->Draw();
    }
}

mr::Mesh::Mesh(const TStaticArray<IGeometry*>& geom, IMaterial* mat)
 : _geom(geom), _mat(mat) {
}

mr::Mesh::~Mesh() {
}

mr::IMesh* mr::Mesh::Create(const TStaticArray<IGeometry*>& geom, IMaterial* mat) {
    return dynamic_cast<mr::IMesh*>(new mr::Mesh(geom, mat));
}
