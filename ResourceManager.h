#pragma once

#ifndef _MR_RESOURCE_MANAGER_H_
#define _MR_RESOURCE_MANAGER_H_

#include <vector>
#include <algorithm>
#include <memory>
#include <string>

namespace MR{
    class ResourceManager;

    class Resource{
    public:
        //Resource name in it's manager
        virtual std::string GetName() = 0;

        //Resource source in filesystem
        virtual std::string GetSource() = 0;

        //Loads resource if it isn't loaded
        virtual bool Load() = 0;

        //Unloads resource if it is loaded
        virtual void UnLoad() = 0;

        //Unloads and then loads resource back
        virtual bool ReLoad() = 0;

        //Check if resource is loaded
        virtual bool IsLoaded() = 0;

        //Gets resource manager of this resource
        virtual ResourceManager* GetManager() = 0;

        //Default constructor
        Resource(MR::ResourceManager* manager, std::string name, std::string source){}
    };

    class ResourceManager{
    public:
        //Adds created resource from manager
        virtual void Add(Resource* res) = 0;

        //Removes created resource from manager
        virtual void Remove(Resource* res) = 0;

        //Creates resource from source
        virtual Resource* Create(std::string name, std::string source) = 0;

        //Creates and loads resource from source
        virtual Resource* CreateAndLoad(std::string name, std::string source) = 0;

        //Returns created resource from source
        virtual Resource* Get(std::string source) = 0;

        //Finds resource by it's name
        virtual Resource* Find(std::string name) = 0;

        //Needs resource by source
        //Creates and loads if not finded
        //Returns loaded resource or nullptr if failed
        virtual Resource* Need(std::string source) = 0;

        //Needs resource list by sources
        //Creates and loads if not finded or element will be nullptr if can't load resource
        //Returns array of pointers
        virtual Resource** Need(std::string* sources, const unsigned int num) = 0;

        //Removes resource
        virtual void Remove(std::string name, std::string source) = 0;

        //Removes all resources
        virtual void RemoveAll() = 0;
    };
}

#endif
