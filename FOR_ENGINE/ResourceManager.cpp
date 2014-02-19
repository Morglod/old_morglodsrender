#include "ResourceManager.hpp"

std::string MR::DirectoryFromFilePath(const std::string& file){
    std::string::size_type it_a = file.find_last_of("\\");
    std::string::size_type it_b = file.find_last_of("/");
    if((it_a == file.npos)&&(it_b == file.npos)) return file;
    if((it_a == file.npos)&&(it_b != file.npos)) return file.substr(0, it_b);
    if((it_a != file.npos)&&(it_b == file.npos)) return file.substr(0, it_a);
    return file.substr(0, std::max(it_a, it_b));
}

MR::Resource* MR::ResourceManager::CreateAndLoad(std::string name, std::string source){
    Resource* r = this->Create(name, source);
    r->Load();
    return r;
}

MR::Resource* MR::ResourceManager::Get(std::string source) {
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return (*it);
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Find(std::string name) {
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Need(std::string source){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return *it;
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

void MR::ResourceManager::Remove(std::string name, std::string source){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name && (*it)->GetSource() == source){
            delete (*it);
            this->_resources.erase(it);
        }
    }
}

void MR::ResourceManager::RemoveAll(){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        delete (*it);
    }
    this->_resources.clear();
}

MR::Resource::Resource(const Resource& r) : OnResourceFreeStateChanged(), _res_free_state(true), _name(""), _source(""), _resource_manager(0), _loaded(false) {}

MR::Resource::Resource(MR::ResourceManager* manager, std::string name, std::string source) : OnResourceFreeStateChanged(), _res_free_state(true), _name(name), _source(source), _resource_manager(manager), _loaded(false) {
}

MR::Resource::~Resource(){}

