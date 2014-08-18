#include "Threads.hpp"

#include <pthread.h>
#include <signal.h>

namespace MR {

/* MUTEX */

void Mutex::Lock(){
    _lastErCode = pthread_mutex_lock((pthread_mutex_t*)_handle);
    if(!_lastErCode) _locked = true;
}

bool Mutex::TryLock(){
    _lastErCode = pthread_mutex_trylock((pthread_mutex_t*)_handle);
    if(!_lastErCode) _locked = true;
    return (!_lastErCode);
}

void Mutex::UnLock(){
    _lastErCode = pthread_mutex_unlock((pthread_mutex_t*)_handle);
    if(!_lastErCode) _locked = false;
}

Mutex::Mutex(const Mutex& m) : _lastErCode(m._lastErCode), _locked(m._locked), _handle(m._handle) {
}

Mutex::Mutex() : _lastErCode(0), _locked(false) {
    _handle = new pthread_mutex_t(PTHREAD_MUTEX_INITIALIZER);
    pthread_mutex_init((pthread_mutex_t*)_handle, NULL);
}

Mutex::~Mutex(){
    pthread_mutex_destroy((pthread_mutex_t*)_handle);
    delete ((pthread_mutex_t*)_handle);
}

/* THREAD */

bool Thread::Start(void* arg){
    _runArg = arg;

    if (pthread_create((pthread_t*)_handle, NULL, threadFunc, (void*)this) != 0) {
        return false;
    }

    return true;
}

void* Thread::threadFunc(void* th) {
    return ((Thread*)th)->Run(((Thread*)th)->_runArg);
}

void* Thread::Join(Thread* thread){
    if(!thread) return NULL;

    void* rptr = 0;
    pthread_join(*((pthread_t*)(thread->_handle)), &rptr);

    return rptr;
}

void Thread::Detach(Thread* thread){
    pthread_detach(*((pthread_t*)(thread->_handle)));
}

bool Thread::IsRunning(){
    return !(pthread_kill(*((pthread_t*)(_handle)), 0));
}

void Thread::ExitThis(void* result){
    pthread_exit(result);
}

Thread* Thread::Self(){
    return new SelfThread(new pthread_t(pthread_self()));
}

bool Thread::operator==(const Thread& thread){
    if( ((pthread_t*)(thread._handle))->p == ((pthread_t*)(_handle))->p ) return true;
    return false;
}

Thread::Thread(const Thread& t) : _handle(t._handle), _runArg(t._runArg) {
}

Thread::Thread(void* handle) : _handle(handle), _runArg(0) {
}

Thread::Thread() : _handle(new pthread_t()), _runArg(0) {

}

Thread::~Thread(){
}

SelfThread::SelfThread(void* handle) : Thread(handle) {
}

void* AsyncHandle::End(){
    return Thread::Join(_thread);
}

AsyncHandle::AsyncHandle(Thread* thread, void* arg, const bool& noErrors) : _noErrors(false), _thread(thread), _arg(arg) {}

AsyncHandle::~AsyncHandle(){
}

class AsyncThread : public Thread {
public:
    AsyncHandle::MethodPtr _method;

    void* Run(void* arg) override {
        return _method(arg);
    }

    AsyncThread(AsyncHandle::MethodPtr method) : Thread(), _method(method) {}
};

AsyncHandle AsyncCall(AsyncHandle::MethodPtr entry, void* arg){
    AsyncThread* thread = new AsyncThread(entry);
    return AsyncHandle(dynamic_cast<Thread*>(thread), arg, thread->Start(arg));
}

}
