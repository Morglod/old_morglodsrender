#pragma once

#ifndef _MR_GL_CONTEXT_H_
#define _MR_GL_CONTEXT_H_

namespace MR{
namespace GL {

class IContext {
public:
    typedef void (*ProcFunc)();

    virtual bool Init() = 0;
    virtual void Destroy() = 0;
    void Free(); //called from Destroy

    virtual void MakeCurrent() = 0; //should call SetCurrent
    virtual void SwapBuffers() = 0;
    virtual bool ExtensionSupported(const char* ext) = 0;
    virtual ProcFunc GetProcAddress(const char* procname) = 0;
};

void SetCurrent(IContext* ctx);
IContext* GetCurrent();

}
}

#endif // _MR_GL_CONTEXT_H_
