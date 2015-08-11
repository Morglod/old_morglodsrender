#pragma once

#ifndef _MR_POST_PROCESS_INTERFACES_H_
#define _MR_POST_PROCESS_INTERFACES_H_

#include "../Utils/Containers.hpp"

namespace mr {

class IShaderProgram;

class IPostprocessFilter {
public:
    virtual void SetShaderProgram(IShaderProgram* sp) = 0;
    virtual IShaderProgram* GetShaderProgram() = 0;
};

class IPostprocessStep {
public:
    virtual void SetFilter(IPostprocessFilter* f) = 0;
    virtual IPostprocessFilter* GetFilter() = 0;
};

class IPostprocessChain {
public:
    virtual mr::TStaticArray<IPostprocessStep*> GetSteps() = 0;
    virtual void SetSteps(const mr::TStaticArray<IPostprocessStep*>& steps) = 0;
};

}

#endif
