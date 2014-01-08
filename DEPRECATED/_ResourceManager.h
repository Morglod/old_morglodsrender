#pragma once

#ifndef _MR_IResource_MANAGER_H_
#define _MR_IResource_MANAGER_H_

#include <vector>
#include <algorithm>
#include <string>
#include <memory>

#include <iostream>

namespace MR{
    class IResourceManager;

    class IResource{
    protected:
        std::string _name = "IResource";
        std::string _source = "";
        IResourceManager* _manager;
    public:
        //-----------------------------
        //Description of loading states
        enum class LoadingState{
            LoadingState_NotLoaded = 0,
            LoadingState_Loading = 1,
            LoadingState_Loaded = 2,
            LoadingState_Unloading = 3,
            LoadingState_Error = 4
        };

        //-------------
        //Load IResource
        virtual void Load() = 0;

        //-----------------------
        //UnLoad and Load methods
        virtual inline void ReLoad(){this->UnLoad();this->Load();}

        //---------------
        //Unload IResource
        virtual void UnLoad() = 0;

        //---------------------------
        //Check if IResource is loaded
        virtual inline bool IsLoaded(){if(this->_current_state == MR::IResource::LoadingState::LoadingState_Loaded) return true; return false;}

        //-----------------
        //Get loading state
        virtual inline LoadingState GetLoadingState(){return this->_current_state;}

        //-----------------
        //Get IResource name
        virtual inline std::string GetName(){return this->_name;}

        virtual inline std::string GetSource(){return this->_source;}

        inline MR::IResource* GetHandle(){return this;}

        IResource(){}
        IResource(MR::IResourceManager* manager, std::string source, std::string name = "IResource");
        virtual ~IResource();

        class IResourceEvents{
        public:
            virtual void LoadingStarted(MR::IResource* res_prt){}
            virtual void LoadingComplete(MR::IResource* res_ptr){}
            virtual void UnloadingStarted(MR::IResource* res_ptr){}
            virtual void UnloadingComplete(MR::IResource* res_ptr){}
            virtual void Error(MR::IResource* res_ptr){}

            IResourceEvents(){}
            virtual ~IResourceEvents(){}
        };

        virtual void AddListener(IResourceEvents* res_listen, bool pass_happened_events = false);
        virtual void RemoveListener(IResourceEvents* res_listen);
    protected:
        std::vector< IResourceEvents* > _listeners;
        LoadingState _current_state = LoadingState::LoadingState_NotLoaded;

        //-------------------------------------------
        //Sends event to all IResourceEvents listeners
        //0 - loadingstarted
        //1 - loadingcomplete
        //2 - unloadingstarted
        //3 - unloadingcomplete
        //4 - error
        virtual void _send_event(int8_t e);
    };

    class IResourceManager{
    protected:
        std::vector< MR::IResource* > _IResources;
    public:
        template<typename IResourceType, typename IResourceTypePtr>
        IResourceTypePtr Create(std::string source, std::string name = "IResource"){
            IResourceTypePtr res = new IResourceType(this, source, name);
            this->_IResources.push_back( res );
            return res;
        }

        virtual void Remove(std::string source);

        virtual void Remove(std::string source, std::string name);

        virtual void RemoveAll();

        //----------------
        //Find res by name
        virtual IResource* Find(std::string name);

        //---------------------------------
        //If can't find it in loaded, loads
        //source - source res outside
        template<typename IResourceType, typename IResourceTypePtr>
        IResourceTypePtr Need(std::string source){
            for(auto it = this->_IResources.begin(); it != this->_IResources.end(); it++){
                if((*it)->GetSource() == source){
                    if( !(*it)->IsLoaded() ) (*it)->Load();
                    return (IResourceTypePtr)(void*)(*it)->GetHandle();
                }
            }
            IResourceTypePtr ptr = this->Create<IResourceType, IResourceTypePtr>(source, "IResource");
            ptr->Load();
            return ptr;
        }

        static inline IResourceManager* GetDefault(){
            static MR::IResourceManager * theSingleInstance = new MR::IResourceManager();
            return theSingleInstance;
        }

        IResourceManager(){}
        virtual ~IResourceManager(){_IResources.clear();}
    };
}

#endif // _MR_IResource_MANAGER_H_
