#include "Mesh.hpp"
#include "../Utils/Log.hpp"
#include "../Materials/MaterialInterfaces.hpp"
#include "GeometryInterfaces.hpp"
#include "../Utils/Debug.hpp"

void MR::Mesh::Draw() {
    auto passes = _mat->GetAllPasses();
    for(size_t im = 0; im < passes.GetNum(); ++im) {
        passes.At(im)->Use();
        for(size_t i = 0; i < _geom.GetNum(); ++i){
            _geom.GetRaw()[i]->Draw();
        }
    }
}

MR::Mesh::Mesh(const StaticArray<IGeometry*>& geom, IMaterial* mat)
 : _geom(geom), _mat(mat) {
}

MR::Mesh::~Mesh() {
}