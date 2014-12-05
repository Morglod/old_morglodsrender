#pragma once

#ifndef _MR_RESOURCE_INTERFACES_H_
#define _MR_RESOURCE_INTERFACES_H_

#include "ResourceType.hpp"

namespace mr {

class IResource {
public:
    virtual ResourceTypePtr GetType() = 0;
    virtual std::string GetName() = 0;
    virtual size_t GetHash() { return 0; }
    virtual bool Equal(IResource* res) { return GetHash() == res->GetHash(); }

    virtual ~IResource() {}
};

}

#endif // _MR_RESOURCE_INTERFACES_H_
