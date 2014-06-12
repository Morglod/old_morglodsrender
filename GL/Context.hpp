#pragma once

#ifndef _MR_GL_CONTEXT_H_
#define _MR_GL_CONTEXT_H_

#include "..//Threads.hpp"

#include <vector>
#include <string>

namespace MR{
namespace GL {

class IGLObject;

class IContext {
public:
    typedef void (*ProcFunc)();

    virtual bool Init(bool multithread = false) = 0;
    virtual void Destroy() = 0;

    //GL CONTEXT

    virtual void MakeCurrent() = 0; //should call SetCurrent
    virtual void SwapBuffers() = 0;
    virtual bool ExtensionSupported(const char* ext) = 0;
    virtual ProcFunc GetProcAddress(const char* procname) = 0;

    virtual void* _GetMultithreadContext() = 0;

    IContext();
    virtual ~IContext();
};

void SetCurrent(IContext* ctx);
IContext* GetCurrent();

}
}

#endif // _MR_GL_CONTEXT_H_
