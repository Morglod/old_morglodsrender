#include "Mesh.hpp"
#include "Log.hpp"
#include "GeometryBuffer.hpp"
#include "Material.hpp"

void MR::Mesh::SetResourceFreeState(const bool& state) {
    if(_res_free_state != state) {
        _res_free_state = state;
        OnResourceFreeStateChanged(this, state);
    }
}

void MR::Mesh::SetMaterials(Material** m, const unsigned int & mnum) {
    if((materials != m) || (materials_num != mnum)) {
        materials = m;
        materials_num = mnum;
        OnMaterialsChanged(this, m, mnum);
    }
}

void MR::Mesh::SetGeomBuffers(GeometryBuffer** gb, const unsigned int & n) {
    if( (geom_buffers != gb) || (geom_buffers_num != n) ) {
        geom_buffers = gb;
        geom_buffers_num = n;
        OnGeometryBuffersChanged(this, gb, n);
    }
}

MR::Mesh::Mesh(GeometryBuffer** gb, unsigned int nm, Material** m, unsigned int mnum) :
    geom_buffers(gb), geom_buffers_num(nm), materials(m), materials_num(mnum), _res_free_state(true) {
}

MR::Mesh::~Mesh() {
    if(_res_free_state) {
        if(geom_buffers) {
            for(unsigned int i = 0; i < this->geom_buffers_num; ++i) {
                delete this->geom_buffers[i];
            }
            free(geom_buffers);
            geom_buffers = nullptr;
        }
        if(materials) {
            for(unsigned int i = 0; i < this->materials_num; ++i) {
                delete this->materials[i];
            }
            free(materials);
            materials = nullptr;
        }
    }
}
