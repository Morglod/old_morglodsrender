#pragma once

#ifndef _MR_RESOURCE_MANAGER_H_
#define _MR_RESOURCE_MANAGER_H_

#include "../Config.hpp"
#include "../Utils/Events.hpp"
#include "../Utils/Threads.hpp"

#include <queue>

namespace MR {

class IContext;

std::string DirectoryFromFilePath(const std::string& file);

class ResourceManager;

class ICPUtoGPULoading {
    friend class ResourceManager;
public:
    virtual void SetAsyncState(const bool& state) { _async_loading = state; }
    virtual bool GetAsyncState() { return _async_loading; }

    /** Calls by ResourceManager if AsyncState or by calling thread if not */

    virtual bool _Loading() { return false; }
    virtual void _UnLoading() { }

    MR::AsyncHandle _async_loading_handle;
    MR::AsyncHandle _async_unloading_handle;

    bool _async_loading = MR_ASYNC_LOADING_DEFAULT;
};

class Resource : public ICPUtoGPULoading {
    friend class ResourceManager;
protected:
    bool _res_free_state;
    std::string _name;
    std::string _source;
    ResourceManager* _resource_manager;
    bool _loaded; /** CHANGES BY RESOURCE MANAGER */

public:
    /** sender - Resource object
     *  arg1 - new state
     */
    mu::Event<Resource*, const bool&> OnResourceFreeStateChanged;

    mu::Event<Resource*> OnLoad;
    mu::Event<Resource*> OnUnLoad;

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
    inline virtual bool GetResourceFreeState() { return _res_free_state; }

    /** Resource name in it's manager
     */
    inline virtual std::string GetName() { return _name; }

    /** Resource source in filesystem
     */
    inline virtual std::string GetSource() { return _source; }

    /** Loads resource if it isn't loaded */
    bool Load();

    /** Unloads resource if it is loaded */
    void UnLoad();

    /** Unloads and then loads resource back
     */
    inline virtual bool ReLoad() {
        this->UnLoad();
        _async_unloading_handle.End();
        return this->Load();
    }

    /** Check if resource is loaded
     */
    inline virtual bool IsLoaded() {
        return _loaded;
    }

    inline void __set_loaded(const bool& state) { _loaded = state; }

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
    virtual ~Resource() {}
};

class ResourceManager {
    friend class Resource;
    friend class ICPUtoGPULoading;
public:
    /** sender - Resource object
     *  arg1 - new state
     */
    mu::Event<ResourceManager*, const bool&> OnDebugMessagesStateChanged;

    /** Turn on/off debug messages
     *  False by default
     */
    inline virtual void SetDebugMessagesState(const bool& state) {
        _debugMessages = state;
        OnDebugMessagesStateChanged(this, state);
    }

    /** Is debug messages turned on?
     *  MR_RESOURCE_MANAGER_DEBUG_MSG_DEFAULT by default */
    inline virtual bool GetDebugMessagesState() { return _debugMessages; }

    /** Adds created resource from manager */
    inline virtual void Add(Resource* res) {
        if(!res) return;
        this->_resources.push_back(res);
        res->_resource_manager = dynamic_cast<MR::ResourceManager*>(this);
    }

    /** Removes created resource from manager */
    inline virtual void Remove(Resource* res) {
        if(!res) return;
        std::vector<Resource*>::iterator it = std::find(this->_resources.begin(), this->_resources.end(), res);
        if(it != this->_resources.end()) {
            delete (*it);
            this->_resources.erase(it);
        }
    }

    /** Creates resource from source */
    virtual Resource* Create(const std::string& name, const std::string& source) = 0;

    /** Creates and loads resource from source */
    virtual Resource* CreateAndLoad(const std::string& name, const std::string& source);

    /** Returns created resource from source */
    virtual Resource* Get(const std::string& source);

    /** Finds resource by it's name */
    virtual Resource* Find(const std::string& name);

    /** Needs resource by source
     *  Creates and loads if not finded
     *  Returns loaded resource or nullptr if failed */
    virtual Resource* Need(const std::string& source);

    //Needs resource list by sources
    //Creates and loads if not finded or element will be nullptr if can't load resource
    //Returns array of pointers
    //virtual Resource** Need(std::string* sources, unsigned int num) = 0;

    /** Removes resource */
    virtual void Remove(const std::string& name, const std::string& source);

    /** Removes all resources */
    virtual void RemoveAll();


    virtual MR::IContext* GetContext() { return _ctx; }
    virtual void SetContext(MR::IContext* ctx) { _ctx = ctx; }

    ResourceManager() : _debugMessages(MR_RESOURCE_MANAGER_DEBUG_MSG_DEFAULT), _resources(), _ctx(nullptr) {}

protected:
    bool _debugMessages;
    std::vector<Resource*> _resources;
    MR::IContext* _ctx;
};

class AsyncResourceTask {
public:
    MR::Resource* res;
    bool load; //if false, unload
};

void AddResourceTask(const AsyncResourceTask& task);
void _MR_RequestGPUThread(MR::IContext* ctx);

}

#endif
