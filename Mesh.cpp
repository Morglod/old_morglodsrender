#include "Mesh.hpp"
#include "Log.hpp"
#include "GeometryBuffer.hpp"
#include "Material.hpp"

MR::Mesh::Mesh(GeometryBuffer** gb, unsigned int nm, Material** m, unsigned int mnum) :
    geom_buffers(gb), geom_buffers_num(nm), materials(m), materials_num(mnum), _res_free_state(true) {
}

MR::Mesh::~Mesh() {
    if(_res_free_state){
        if(geom_buffers){
            for(unsigned int i = 0; i < this->geom_buffers_num; ++i) {
                delete this->geom_buffers[i];
            }
            free(geom_buffers);
            geom_buffers = nullptr;
        }
        if(materials){
            for(unsigned int i = 0; i < this->materials_num; ++i) {
                delete this->materials[i];
            }
            free(materials);
            materials = nullptr;
        }
    }
}
