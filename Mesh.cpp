#include "Mesh.hpp"

bool MR::Mesh::Load() {
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") loading");
    if(this->_source != "") {
        if(!MR::ImportMoGeom(this->_source, this->geom_buffers, this->geom_buffers_num, true, this->_manager->debugMessages)) {
            MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") importing failed.");
            this->_loaded = false;
            return false;
        }
    } else if(this->_manager->debugMessages) {
        MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") load failed. Source is null");
        this->_loaded = false;
        return false;
    }

    this->_loaded = true;
    return true;
}

void MR::Mesh::UnLoad() {
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") unloading");

    if(_res_free_state){
        if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") -> ResFreeState is on, deleting data");

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
        if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") -> ResFreeState is off");
    }
}

bool MR::Mesh::ReLoad() {
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") reloading");
    this->UnLoad();
    return this->Load();
}

MR::Mesh::Mesh(MR::MeshManager* manager, std::string name, std::string source) : MR::Resource(manager, name, source), _name(name), _source(source), _manager(manager) {
}

MR::Mesh::~Mesh() {
    if(this->_manager->debugMessages) MR::Log::LogString("Mesh "+this->_name+" ("+this->_source+") deleting");
    this->UnLoad();
}

//RESOURCE MANAGER
MR::Resource* MR::MeshManager::Create(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") creating");
    Mesh * m = new Mesh(this, name, source);
    this->_resources.push_back(m);
    return m;
}

MR::Resource* MR::MeshManager::CreateAndLoad(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") creating and loading");
    Resource* m = this->Create(name, source);
    m->Load();
    return m;
}

MR::Resource* MR::MeshManager::Get(std::string source) {
    if(this->debugMessages) MR::Log::LogString("MeshManager ("+source+") getting");
    for(std::vector<Mesh*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it) {
        if( (*it)->GetSource() == source ) return *it;
    }
    return nullptr;
}
MR::Resource* MR::MeshManager::Find(std::string name) {
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" searching");
    for(std::vector<Mesh*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it) {
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}

MR::Resource* MR::MeshManager::Need(std::string source) {
    if(this->debugMessages) MR::Log::LogString("MeshManager ("+source+") needed");
    for(std::vector<Mesh*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it) {
        if( (*it)->GetSource() == source ) return *it;
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

void MR::MeshManager::Remove(std::string name, std::string source) {
    if(this->debugMessages) MR::Log::LogString("MeshManager "+name+" ("+source+") removing");
    for(std::vector<Mesh*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it) {
        if( (*it)->GetName() == name && (*it)->GetSource() == source) {
            delete *it;
            this->_resources.erase(it);
        }
    }
}

void MR::MeshManager::RemoveAll() {
    if(this->debugMessages) MR::Log::LogString("MeshManager removing all");
    for(std::vector<Mesh*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it) {
        delete *it;
        this->_resources.erase(it);
        --it;
    }
}
