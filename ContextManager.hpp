#pragma once

#ifndef _MR_CONTEXT_H_
#define _MR_CONTEXT_H_

namespace mr{

class IContext {
public:
    virtual void Destroy() = 0;
    virtual void MakeCurrent() = 0;
    virtual void SwapBuffers() = 0;

    virtual void GetWindowSizes(int& width, int& height) = 0;
};

class IContextManager {
public:
    typedef void (*ProcFunc)();
    static IContextManager* Current;

    virtual IContext* GetDefaultContext() = 0;
    virtual unsigned int GetContextsNum() = 0;
    virtual IContext* GetContext(unsigned int const& contextIndex) = 0;
    virtual void MakeNullContextCurrent() = 0; //MakeCurrent(0);
    virtual void Destroy();

    virtual bool IsMultithread() = 0;

    virtual bool ExtensionSupported(const char* ext) = 0;
    virtual ProcFunc GetProcAddress(const char* procname) = 0;
};

inline bool AnyContextAlive() { return (IContextManager::Current != nullptr); }

}

#endif // _MR_GL_CONTEXT_H_
