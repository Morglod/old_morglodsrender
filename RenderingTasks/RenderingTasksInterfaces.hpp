#pragma once

#ifndef _MR_RENDERING_TASKS_INTERFACES_H_
#define _MR_RENDERING_TASKS_INTERFACES_H_

namespace MR {

class IShaderProgram;

class IRenderingTask {
public:
    virtual IRenderingTask* GetPrev() { return nullptr; }
    virtual IShaderProgram* ShaderProgram() { IRenderingTask* rt = GetPrev(); return ((rt != nullptr) ? rt->ShaderProgram() : nullptr); }
    virtual bool DepthTest() { IRenderingTask* rt = GetPrev(); return ((rt != nullptr) ? rt->DepthTest() : false); }
};

}

#endif
