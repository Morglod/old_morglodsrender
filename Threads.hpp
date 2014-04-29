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

    inline int GetLastError() {
        int a = _lastErCode;
        _lastErCode = 0;
        return a;
    }

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
    typedef void* (*ThreadEntryPtr)(void*);

    bool Start(void* arg, const bool& joinable = true);
    bool IsRunning();

    static void* Join(Thread* thread);
    static void Detach(Thread* thread);
    static void ExitThis(void* result);
    static Thread Self();

    bool operator==(const Thread& thread);

    Thread(const Thread& t);
    Thread(ThreadEntryPtr tentry);
    ~Thread();
protected:
    Thread(void* handle);
    void* _handle;
    ThreadEntryPtr _entry;
    bool _joinable;

    class ThreadArg {
    public:
        ThreadEntryPtr entry;
        void* arg;
        void* returned;
        ThreadArg(ThreadEntryPtr e, void* a, void* r) : entry(e), arg(a), returned(r) {}
    };

    static void* ThreadFunction(void* threadArg);
};

class AsyncHandle {
public:
    inline Thread* GetThread() { return _thread; }
    void* End();
    void* GetArg() { return _arg; }
    bool NoErrors() { return _noErrors; }

    AsyncHandle(Thread* thread, void* arg, const bool& noErrors);
    ~AsyncHandle();
protected:
    bool _noErrors;
    Thread* _thread;
    void* _arg;
};

AsyncHandle AsyncCall(Thread::ThreadEntryPtr entry, void* arg);

}

#endif // _MR_THREAD_H_
