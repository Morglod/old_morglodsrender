#include "ResourceManager.h"
#include "Log.h"

void MR::IResource::AddListener(MR::IResource::IResourceEvents* res_listen, bool pass_happened_events){
    auto it = std::find(this->_listeners.begin(), this->_listeners.end(), res_listen);
    if(it != this->_listeners.end()) return;

    this->_listeners.push_back(res_listen);

    if(pass_happened_events){
        MR::IResource::LoadingState ls = this->GetLoadingState();
        if(ls == MR::IResource::LoadingState::LoadingState_Error) res_listen->Error(this);
        else if(ls == MR::IResource::LoadingState::LoadingState_Loaded) res_listen->LoadingComplete(this);
        else if(ls == MR::IResource::LoadingState::LoadingState_Loading) res_listen->LoadingStarted(this);
        else if(ls == MR::IResource::LoadingState::LoadingState_Unloading) res_listen->UnloadingStarted(this);
    }
}

void MR::IResource::RemoveListener(MR::IResource::IResourceEvents* res_listen){
    auto it = std::find(this->_listeners.begin(), this->_listeners.end(), res_listen);
    if(it != this->_listeners.end()) this->_listeners.erase(it);
}

void MR::IResource::Load(){
    this->_current_state = MR::IResource::LoadingState::LoadingState_Loading;
    this->_send_event(0);
    try{
        this->_send_event(1);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Loaded;
    }
    catch(std::exception & e){
        this->_send_event(4);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Error;
        MR::Log::LogException(e);
    }
}

void MR::IResource::UnLoad(){
    std::cout << "RES[UNLOAD]";
    this->_current_state = MR::IResource::LoadingState::LoadingState_Unloading;
    this->_send_event(2);
    try{
        this->_current_state = MR::IResource::LoadingState::LoadingState_Loaded;
        this->_send_event(3);
    }
    catch(std::exception & e){
        this->_send_event(4);
        this->_current_state = MR::IResource::LoadingState::LoadingState_Error;
        MR::Log::LogException(e);
    }
}

//0 - loadingstarted
//1 - loadingcomplete
//2 - unloadingstarted
//3 - unloadingcomplete
//4 - error
void MR::IResource::_send_event(int8_t e){
    for(auto it = this->_listeners.begin(); it != this->_listeners.end(); it++){
                if(e == 0) (*it)->LoadingStarted(this);
        else    if(e == 1) (*it)->LoadingComplete(this);
        else    if(e == 2) (*it)->UnloadingStarted(this);
        else    if(e == 3) (*it)->UnloadingComplete(this);
        else    if(e == 4) (*it)->Error(this);
    }
}

MR::IResource::IResource(MR::IResourceManager* m, std::string source, std::string name){
    this->_manager = m;
    this->_name = name;
    this->_source = _source;
}

MR::IResource::~IResource(){
    this->_listeners.clear();
}

//--------------------------------------------------------------------------------------------------------------------------------------

void MR::IResourceManager::Remove(std::string name){
    for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++)
        if((*it)->GetName() == name){
            delete (*it);
            this->_IResources.erase(it);
        }
}

void MR::IResourceManager::Remove(std::string name, std::string source){
    for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++)
        if((*it)->GetName() == name && (*it)->GetSource() == source){
            delete (*it);
            this->_IResources.erase(it);
        }
}

void MR::IResourceManager::RemoveAll(){
    for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++){
        delete (*it);
        this->_IResources.erase(it);
    }
}

//----------------
//Find res by name
MR::IResource* MR::IResourceManager::Find(const std::string name){
    for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++)
        if((*it)->GetName() == name)
            return (*it);

    return nullptr;
}

//---------------------------------
//If can't find it in loaded, loads
//source - source res outside
/*template<class IResourceType>
MR::IResourcePtr MR::IResourceManager::Need(const std::string source){
    for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++){
        if((*it)->GetSource() == source) return (*it);
    }
    return this->Create<IResourceType>(source, "IResource");
}*/
