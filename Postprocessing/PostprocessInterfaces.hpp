#pragma once

#ifndef _MR_POST_PROCESS_INTERFACES_H_
#define _MR_POST_PROCESS_INTERFACES_H_

#include "../Utils/Containers.hpp"

namespace MR {

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
    virtual MR::StaticArray<IPostprocessStep*> GetSteps() = 0;
    virtual void SetSteps(const MR::StaticArray<IPostprocessStep*>& steps) = 0;
};

}

#endif
