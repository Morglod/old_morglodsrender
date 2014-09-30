#pragma once

#ifndef _MR_THREADS_H_
#define _MR_THREADS_H_

namespace MR {

class Mutex {
public:
    virtual void Lock();
    virtual bool TryLock();
    virtual void UnLock();

    inline bool IsLocked() { return _locked; }
    inline int GetErrorCode() { return _lastErCode; }

    Mutex(const Mutex& m);
    Mutex();
    ~Mutex();
private:
    int _lastErCode;
    bool _locked;
    void* _handle;
};

class Thread {
public:
    virtual int Run() = 0;

    bool Start(void* argPtr);
    bool IsRunning();

    static int Join(Thread* thread);
    static void Detach(Thread* thread);
    static void ExitThis(int result);
    static Thread* Self();

    bool operator==(const Thread& thread);

    Thread();
    virtual ~Thread();

protected:
    static void* threadFunc(void* th);
    Thread(void* handle);
    Thread(const Thread& t);

    void* _handle;
    void* arg;
    int _returned;
};

class SelfThread : public Thread {
public:
    int Run() override { return 0; }
    SelfThread(void* handle);
};

class AsyncHandle {
public:
    typedef int (*MethodPtr)(void* arg);

    inline Thread* GetThread() { return _thread; }
    int End();
    void* GetArg() { return _arg; }
    bool NoErrors() { return _noErrors && _thread; }

    AsyncHandle() {}
    AsyncHandle(Thread* thread, void* arg, const bool& noErrors);
    ~AsyncHandle();
protected:
    bool _noErrors = true;
    Thread* _thread = 0;
    void* _arg;
};

AsyncHandle AsyncCall(AsyncHandle::MethodPtr entry, void* arg);

}

#endif // _MR_THREAD_H_
