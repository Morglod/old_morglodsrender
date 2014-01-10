#include "Mesh.hpp"

bool MR::Mesh::Load() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") loading", MR_LOG_LEVEL_INFO);
    if(this->_source != "") {
        if(!MR::ImportMoGeom(this->_source, this->geom_buffers, this->geom_buffers_num, true, this->_resource_manager->GetDebugMessagesState())) {
            MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") importing failed.", MR_LOG_LEVEL_ERROR);
            this->_loaded = false;
            return false;
        }
    } else if(this->_resource_manager->GetDebugMessagesState()) {
        MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") load failed. Source is null", MR_LOG_LEVEL_ERROR);
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}

void MR::Mesh::UnLoad() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") unloading", MR_LOG_LEVEL_INFO);

    if(_res_free_state){
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data", MR_LOG_LEVEL_INFO);

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
    else {
        if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") -> ResFreeState is off", MR_LOG_LEVEL_INFO);
    }
}

MR::Mesh::Mesh(MR::MeshManager* manager, std::string name, std::string source) : MR::Resource(manager, name, source), geom_buffers_num(0), materials_num(0) {
}

MR::Mesh::~Mesh() {
    if(this->_resource_manager->GetDebugMessagesState()) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") deleting", MR_LOG_LEVEL_INFO);
    UnLoad();
}

//RESOURCE MANAGER
MR::Resource* MR::MeshManager::Create(std::string name, std::string source) {
    if(this->GetDebugMessagesState()) MR::Log::LogString("MeshManager "+name+" ("+source+") creating", MR_LOG_LEVEL_INFO);
    Mesh * m = new Mesh(this, name, source);
    this->_resources.push_back(m);
    return m;
}
