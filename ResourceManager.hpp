#pragma once

#ifndef _MR_RESOURCE_MANAGER_H_
#define _MR_RESOURCE_MANAGER_H_

#include "Config.hpp"
#include "Events.hpp"

namespace MR{

    std::string DirectoryFromFilePath(const std::string& file);

    class ResourceManager;

    class Resource {
    protected:
        bool _res_free_state;
        std::string _name;
        std::string _source;
        ResourceManager* _resource_manager;
        bool _loaded;

    public:
        /** sender - Resource object
         *  arg1 - new state
         */
        MR::Event<const bool&> OnResourceFreeStateChanged;

        /** If true, resource data will be destroyed after unloading/deleting resource
         *  For example geometry buffers after unloading/deleting Mesh resource
         *  True by default
         */
        inline virtual void SetResourceFreeState(const bool& state) {
            _res_free_state = state;
            OnResourceFreeStateChanged(this, state);
        }

        /** Get state
         *  If true, resource data will be destroyed after unloading/deleting resource
         *  For example geometry buffers after unloading/deleting Mesh resource
         *  True by default
         */
        inline virtual bool GetResourceFreeState() {
            return _res_free_state;
        }

        /** Resource name in it's manager
         */
        inline virtual std::string GetName() {
            return _name;
        }

        /** Resource source in filesystem
         */
        inline virtual std::string GetSource() {
            return _source;
        }

        /** Loads resource if it isn't loaded
         */
        virtual bool Load() = 0;

        /** Unloads resource if it is loaded
         */
        virtual void UnLoad() = 0;

        /** Unloads and then loads resource back
         */
        inline virtual bool ReLoad() {
            this->UnLoad();
            return this->Load();
        }

        /** Check if resource is loaded
         */
        inline virtual bool IsLoaded() {
            return _loaded;
        }

        /** Gets resource manager of this resource
         */
        inline virtual ResourceManager* GetManager() {
            return _resource_manager;
        }

        /** Default constructor
         *  manager - Resource manager
         *  name - name of resource instance in resource manager
         *  source - file source (path to file)
         */
        Resource(MR::ResourceManager* manager, const std::string& name, const std::string& source) : _res_free_state(true), _name(name), _source(source), _resource_manager(manager), _loaded(false) {}
        virtual ~Resource(){}
    };

    class ResourceManager {
    protected:
        bool _debugMessages;
        std::vector<Resource*> _resources;

    public:
        /** sender - Resource object
         *  arg1 - new state
         */
        MR::Event<const bool&> OnDebugMessagesStateChanged;

        /** Turn on/off debug messages
         *  False by default
         */
        inline virtual void SetDebugMessagesState(const bool& state){
            _debugMessages = state;
            OnDebugMessagesStateChanged(this, state);
        }

        /** Is debug messages turned on?
         *  False by default
         */
        inline virtual bool GetDebugMessagesState(){
            return _debugMessages;
        }

        /** Adds created resource from manager
         */
        inline virtual void Add(Resource* res){
            this->_resources.push_back(res);
        }

        /** Removes created resource from manager
         */
        inline virtual void Remove(Resource* res){
            std::vector<Resource*>::iterator it = std::find(this->_resources.begin(), this->_resources.end(), res);
            if(it != this->_resources.end()){
                delete (*it);
                this->_resources.erase(it);
            }
        }

        /** Creates resource from source
         */
        virtual Resource* Create(const std::string& name, const std::string& source) = 0;

        /** Creates and loads resource from source
         */
        virtual Resource* CreateAndLoad(const std::string& name, const std::string& source);

        /** Returns created resource from source
         */
        virtual Resource* Get(const std::string& source);

        /** Finds resource by it's name
         */
        virtual Resource* Find(const std::string& name);

        /** Needs resource by source
         *  Creates and loads if not finded
         *  Returns loaded resource or nullptr if failed
         */
        virtual Resource* Need(const std::string& source);

        //Needs resource list by sources
        //Creates and loads if not finded or element will be nullptr if can't load resource
        //Returns array of pointers
        //virtual Resource** Need(std::string* sources, unsigned int num) = 0;

        /** Removes resource
         */
        virtual void Remove(const std::string& name, const std::string& source);

        /** Removes all resources
         */
        virtual void RemoveAll();

        ResourceManager() : _debugMessages(false) {}
    };
}

#endif
