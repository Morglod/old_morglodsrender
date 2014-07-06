#include "Mesh.hpp"
#include "Utils/Log.hpp"
#include "Buffers/GeometryBufferV2.hpp"
#include "Material.hpp"

void MR::Mesh::SetResourceFreeState(const bool& state) {
    if(_res_free_state != state) {
        _res_free_state = state;
        OnResourceFreeStateChanged(this, state);
    }
}

void MR::Mesh::SetMaterial(Material* m) {
    if(material != m) {
        material = m;
        OnMaterialChanged(this, m);
    }
}

void MR::Mesh::SetGeoms(IGeometry** gb, const unsigned int & n) {
    if( (geoms != gb) || (geoms_num != n) ) {
        geoms = gb;
        geoms_num = n;
        OnGeometryChanged(this, gb, n);
    }
}

MR::Mesh::Mesh(IGeometry** gb, unsigned int nm, Material* m) :
    geoms(gb), geoms_num(nm), material(m), _res_free_state(true) {
}

MR::Mesh::~Mesh() {
    if(_res_free_state) {
        if(geoms) {
            for(size_t i = 0; i < this->geoms_num; ++i) {
                delete this->geoms[i];
            }
            free(geoms);
            geoms = nullptr;
        }
        if(material) {
            delete material;
            material = nullptr;
        }
    }
}
