#pragma once

#ifndef _MR_RESOURCE_MANAGER_H_
#define _MR_RESOURCE_MANAGER_H_

#include <functional>

namespace mr {

class ResourcesRequest {
public:
    TStaticArray<std::string> _resNames;
};

class ResourceManager {
public:
    ResourceManager& GetInstance() { return _inst; }
    ResourceManager(ResourceManager const& rm) = delete;
private:
    ResourceManager();
    ~ResourceManager();

    /*std::hash<std::string> _make_hash;
    std::map<size_t, IResource*> _res;
    std::map<size_t, ResourceType> _res_types;*/

    static ResourceManager _inst;
};

}

#endif // _MR_RESOURCE_MANAGER_H_
