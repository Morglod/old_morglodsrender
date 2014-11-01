#pragma once

#ifndef _MR_RESOURCE_TYPE_H_
#define _MR_RESOURCE_TYPE_H_

#include <string>
#include "../Utils/Pointers.hpp"

namespace MR {

class ResourceType {
public:
    std::string GetName() { return _name; }
    std::string GetDescription() { return _description; }
    size_t GetHash() { return _hash; }
    bool operator == (ResourceType const& t) { return _hash == t._hash; }
    ResourceType& operator = (ResourceType const& rt);

    ResourceType(std::string const& name, std::string const& descr = "");
    ResourceType(ResourceType const& copy_type);
    virtual ~ResourceType();
private:
    std::string _name, _description;
    size_t _hash;
};
typedef HandlePtr<ResourceType> ResourceTypePtr;

}

#endif
