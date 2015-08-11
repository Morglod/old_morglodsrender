#pragma once

#ifndef _MR_PLUGIN_SYSTEM_H_
#define _MR_PLUGIN_SYSTEM_H_

#include <vector>
#include <map>
#include <string>

namespace mr {

class Plugin;
class PluginMethod;
class PluginSystem;

class PluginMethod {
public:
    inline std::string GetName() { return _name; }
    inline virtual void* Invoke(void* arg) { return 0; }
protected:
    std::string _name;
    Plugin* _plugin;
};

class Plugin {
public:
    virtual bool Init(PluginSystem* sys);
    virtual void Free();

    inline std::string GetName() { return _name; }
    inline std::string GetDescr() { return _description; }
    inline PluginSystem* GetPluginSystem() { return _sys; }
    inline PluginMethod* GetMethod(const std::string& name) {
        if(_methods.count(name) == 0) return 0;
        return _methods[name];
    }

    Plugin();
    virtual ~Plugin();
protected:
    std::string _name, _description;
    PluginSystem* _sys;
    std::map<std::string, PluginMethod*> _methods;
};

class PluginSystem {
    friend class Plugin;
public:
    inline void Add(Plugin* p) { _plugins.push_back(p); }
    inline void Remove(Plugin* p) { _plugins.push_back(p); }
    inline std::vector<Plugin*>& GetPluginsList() { return _plugins; }
    inline std::vector<Plugin*>* GetPluginsListPtr() { return &_plugins; }
    bool LoadFromModule(const std::string& path, Plugin** ptr_p); //calls GetInstance from dll module
protected:
    std::vector<Plugin*> _plugins;
};

}

extern "C" {
mr::Plugin* GetPluginInstance(mr::PluginSystem* sys); //rewrite method for your plugin; should add Initializated new instance to PluginSystem
}

typedef mr::Plugin* (*GetPluginInstanceProc)(mr::PluginSystem* sys);

#endif // _MR_PLUGIN_SYSTEM_H_
