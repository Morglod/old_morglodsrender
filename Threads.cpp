#include "Threads.hpp"

#include <pthread.h>
#include <signal.h>

namespace MR {

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

bool Thread::Start(void* arg, const bool& joinable){
    _joinable = joinable;

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if(joinable)    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    else            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    ThreadArg* thrArg = new ThreadArg(_entry, arg, 0);

    if (pthread_create((pthread_t*)_handle, &attr, ThreadFunction, thrArg) != 0) {
        pthread_attr_destroy(&attr);
        return false;
    }

    pthread_attr_destroy(&attr);
    return true;
}

void* Thread::Join(Thread* thread){
    if(!thread) return NULL;

    ThreadArg* res = new ThreadArg(0,0,0);
    pthread_join(*((pthread_t*)thread->_handle), (void**)&res);
    if(res) return res->returned;
    else return 0;
}

void Thread::Detach(Thread* thread){
    pthread_detach(*((pthread_t*)thread->_handle));
}

bool Thread::IsRunning(){
    return !(pthread_kill(*((pthread_t*)_handle), 0));
}

void Thread::ExitThis(void* result){
    pthread_exit(result);
}

Thread Thread::Self(){
    pthread_t t = pthread_self();
    return Thread(&t);
}

bool Thread::operator==(const Thread& thread){
    if( (*((pthread_t*)thread._handle)).p == (*((pthread_t*)_handle)).p ) return true;
    return false;
}

Thread::Thread(const Thread& t) : _handle(t._handle), _entry(t._entry), _joinable(t._joinable) {
}

Thread::Thread(void* handle) : _handle(handle), _entry(0), _joinable(false) {
}

Thread::Thread(ThreadEntryPtr entry) : _handle(new pthread_t()), _entry(entry), _joinable(false) {

}

Thread::~Thread(){
}

void* Thread::ThreadFunction(void* threadArg){
    ThreadArg* arg = (ThreadArg*)threadArg;
    if(arg->entry == nullptr) {
        arg->returned = 0;
        return NULL;
    }
    arg->returned = arg->entry(arg->arg);
    return arg->returned;
}

void* AsyncHandle::End(){
    return Thread::Join(_thread);
}

AsyncHandle::AsyncHandle(Thread* thread, void* arg, const bool& noErrors) : _noErrors(false), _thread(thread), _arg(arg) {}

AsyncHandle::~AsyncHandle(){
}

AsyncHandle AsyncCall(Thread::ThreadEntryPtr entry, void* arg){
    Thread* thread = new Thread(entry);
    return AsyncHandle(thread, arg, thread->Start(arg, true));
}

}
