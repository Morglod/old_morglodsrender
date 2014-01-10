#include "ResourceManager.hpp"

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
