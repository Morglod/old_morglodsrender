#include "ResourceManager.hpp"
#include "../Utils/Log.hpp"
#include "../Context.hpp"

#include <GL/glew.h>

std::string MR::DirectoryFromFilePath(const std::string& file){
    std::string::size_type it_a = file.find_last_of("\\");
    std::string::size_type it_b = file.find_last_of("/");
    if((it_a == file.npos)&&(it_b == file.npos)) return file;
    if((it_a == file.npos)&&(it_b != file.npos)) return file.substr(0, it_b);
    if((it_a != file.npos)&&(it_b == file.npos)) return file.substr(0, it_a);
    return file.substr(0, std::max(it_a, it_b));
}

std::vector<MR::AsyncResourceTask> _res_async_tasks;

void MR::AddResourceTask(const MR::AsyncResourceTask& task) {
    _res_async_tasks.push_back(task);
}

bool _async_thread_running = false;

int __MR_RESOURCE_ASYNC_THREAD(void* icontext) {
    _async_thread_running = true;

    ((MR::IContext*)icontext)->MakeMultithreadCurrent();

    size_t s = _res_async_tasks.size();
    size_t cur = 0;

    while(s > cur) {
        MR::AsyncResourceTask art = _res_async_tasks[cur];
        MR::Resource* res = art.res;

        if(art.load) {
            res->__set_loaded(res->_Loading());
        } else {
            res->_UnLoading();
            res->__set_loaded(false);
        }

        glFlush();

        ++cur;
        s = _res_async_tasks.size();
    }
    _res_async_tasks.clear();

    ((MR::IContext*)icontext)->MakeNullCurrent();
    _async_thread_running = false;

    return 1;
}

void MR::_MR_RequestGPUThread(MR::IContext* ctx) {
    if(!_async_thread_running) {
        _async_thread_running = true;
        MR::AsyncCall(__MR_RESOURCE_ASYNC_THREAD, ctx);
    }
}

bool MR::Resource::Load() {
    if(GetAsyncState() && _resource_manager->GetContext()->IsMultithread()) {
        AddResourceTask( MR::AsyncResourceTask {this, true} );
        _MR_RequestGPUThread(_resource_manager->GetContext());
        return true;
    } else {
        __set_loaded(_Loading());
        return _loaded;
    }
}

void MR::Resource::UnLoad() {
    if(GetAsyncState() && _resource_manager->GetContext()->IsMultithread()) {
        AddResourceTask( MR::AsyncResourceTask {this, false} );
        _MR_RequestGPUThread(_resource_manager->GetContext());
    } else {
        _UnLoading();
    }
    _loaded = false;
}

MR::Resource* MR::ResourceManager::CreateAndLoad(const std::string& name, const std::string& source){
    Resource* r = this->Create(name, source);
    r->Load();
    return r;
}

MR::Resource* MR::ResourceManager::Get(const std::string& source) {
    for(size_t i = 0; i < _resources.size(); ++i){
        if( _resources[i]->GetSource() == source ) return _resources[i];
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Find(const std::string& name) {
    for(size_t i = 0; i < _resources.size(); ++i){
        if( (_resources[i])->GetName() == name ) return _resources[i];
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Need(const std::string& source){
    for(size_t i = 0; i < _resources.size(); ++i){
        if( (_resources[i])->GetSource() == source ) return _resources[i];
    }
    return this->CreateAndLoad("AutoLoaded_"+source, source);
}

void MR::ResourceManager::Remove(const std::string& name, const std::string& source){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name && (*it)->GetSource() == source){
            delete (*it);
            this->_resources.erase(it);
            return;
        }
    }
}

void MR::ResourceManager::RemoveAll(){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        delete (*it);
    }
    this->_resources.clear();
}
