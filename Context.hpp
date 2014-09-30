#pragma once

#ifndef _MR_CONTEXT_H_
#define _MR_CONTEXT_H_

namespace MR{

class IContext {
public:
    static IContext* Current;

    typedef void (*ProcFunc)();

    virtual void Destroy() = 0;

    virtual void MakeCurrent() = 0;
    virtual void MakeMultithreadCurrent() = 0;
    virtual void MakeNullCurrent() = 0; //MakeCurrent(0);

    virtual bool IsMultithread() = 0; //return false, if no multithreaded context

    virtual void SwapBuffers() = 0;
    virtual bool ExtensionSupported(const char* ext) = 0;
    virtual ProcFunc GetProcAddress(const char* procname) = 0;

    virtual void GetWindowSizes(int* width, int* height) = 0;

    IContext() {}
    virtual ~IContext() {}
};

inline bool AnyContextAlive() { return (IContext::Current != nullptr); }

}

#endif // _MR_GL_CONTEXT_H_
