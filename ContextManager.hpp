#pragma once

namespace mr{

class IContext {
public:
    virtual void Destroy() = 0;
    virtual void MakeCurrent() = 0;
    virtual void SwapBuffers() = 0;

    virtual void GetWindowSizes(int& width, int& height) = 0;

    virtual ~IContext();
};

class IContextManager {
public:
    typedef void (*ProcFunc)();

    static IContextManager* GetCurrent();
    static void SetCurrent(IContextManager*);

    virtual IContext* GetDefaultContext() = 0;
    virtual unsigned int GetContextsNum() = 0;
    virtual IContext* GetContext(unsigned int const& contextIndex) = 0;
    virtual void MakeNullContextCurrent() = 0; //MakeCurrent(0);
    virtual void Destroy() = 0;

    virtual bool IsMultithread() = 0;

    virtual bool ExtensionSupported(const char* ext) = 0;
    virtual ProcFunc GetProcAddress(const char* procname) = 0;

    virtual ~IContextManager();
};

inline bool AnyContextAlive() { return (IContextManager::GetCurrent() != nullptr); }

}
