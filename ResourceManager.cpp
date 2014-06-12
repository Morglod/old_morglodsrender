#include "ResourceManager.hpp"
#include "Log.hpp"

std::string MR::DirectoryFromFilePath(const std::string& file){
    std::string::size_type it_a = file.find_last_of("\\");
    std::string::size_type it_b = file.find_last_of("/");
    if((it_a == file.npos)&&(it_b == file.npos)) return file;
    if((it_a == file.npos)&&(it_b != file.npos)) return file.substr(0, it_b);
    if((it_a != file.npos)&&(it_b == file.npos)) return file.substr(0, it_a);
    return file.substr(0, std::max(it_a, it_b));
}

void* MR::ResourceManager::__MR_RESOURCE_ASYNC_CPU_LOAD(void* handle) {
    MR::Resource* res = (MR::Resource*)handle;
    return (void*)(int)(res->_CpuLoading());
}

void* MR::ResourceManager::__MR_RESOURCE_ASYNC_CPU_UNLOAD(void* handle) {
    MR::Resource* res = (MR::Resource*)handle;
    res->_CpuUnLoading();
    return 0;
}

class __AsyncGPUArg {
public:
    MR::Resource* res;
    void* glfw_multithreadedWindow;
};

/*
void* MR::ResourceManager::__MR_RESOURCE_ASYNC_GPU_LOAD(void* handle) {
    __AsyncGPUArg* arg = (__AsyncGPUArg*)handle;
    return (void*)(int)(arg->res->_GpuLoading());
}

void* MR::ResourceManager::__MR_RESOURCE_ASYNC_GPU_UNLOAD(void* handle) {
    __AsyncGPUArg* arg = (__AsyncGPUArg*)handle;
    arg->res->_GpuUnLoading();
    return 0;
}*/

bool MR::Resource::Load() {
    if(GetAsyncCpuState()) {
        _async_cpu_loading_handle = MR::AsyncCall(MR::ResourceManager::__MR_RESOURCE_ASYNC_CPU_LOAD, this);
        return true;
    } else {
        return _CpuLoading();
    }
}

void MR::Resource::UnLoad() {
    if(GetAsyncCpuState()) {
        _async_cpu_unloading_handle = MR::AsyncCall(MR::ResourceManager::__MR_RESOURCE_ASYNC_CPU_UNLOAD, this);
    } else {
        _CpuUnLoading();
    }
    _loaded = false;
}

void MR::Resource::RequestGPULoad(){
    if(GetAsyncCpuState()) {
        GetManager()->_AsyncGpuLoadPush(this);
    } else {
        _GpuLoading();
    }
}

void MR::Resource::RequestGPUUnLoad(){
    if(GetAsyncCpuState()) {
        GetManager()->_AsyncGpuUnLoadPush(this);
    } else {
        _GpuUnLoading();
    }
}

void MR::ResourceManager::_AsyncGpuLoadPush(MR::Resource* res){
    _async_gpu_need_to_load.push(res);
}

void MR::ResourceManager::_AsyncGpuUnLoadPush(MR::Resource* res){
    _async_gpu_need_to_unload.push(res);
}

MR::Resource* MR::ResourceManager::CreateAndLoad(const std::string& name, const std::string& source){
    Resource* r = this->Create(name, source);
    r->Load();
    return r;
}

MR::Resource* MR::ResourceManager::Get(const std::string& source) {
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return (*it);
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Find(const std::string& name) {
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetName() == name ) return *it;
    }
    return nullptr;
}

MR::Resource* MR::ResourceManager::Need(const std::string& source){
    for(std::vector<Resource*>::iterator it = this->_resources.begin(); it != this->_resources.end(); ++it){
        if( (*it)->GetSource() == source ) return *it;
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

void MR::ResourceManager::AsyncLoadGPU() {
    while( !_async_gpu_need_to_load.empty() ) {
        _async_gpu_need_to_load.front()->_loaded = false;
        if( _async_gpu_need_to_load.front()->_async_cpu_loading_handle.End() != 0)
            if(_async_gpu_need_to_load.front()->_GpuLoading()) {
                _async_gpu_need_to_load.front()->_loaded = true;
                _async_gpu_need_to_load.front()->OnLoad(_async_gpu_need_to_load.front());
            }
        _async_gpu_need_to_load.pop();

        if(_async_one_res_per_update) return;
    }
}

void MR::ResourceManager::AsyncUnLoadGPU() {
    while( !_async_gpu_need_to_unload.empty() ) {
        _async_gpu_need_to_unload.front()->_async_cpu_unloading_handle.End();
        _async_gpu_need_to_unload.front()->_GpuUnLoading();
        _async_gpu_need_to_unload.front()->OnUnLoad(_async_gpu_need_to_unload.front());
        _async_gpu_need_to_unload.pop();

        if(_async_one_res_per_update) return;
    }
}
