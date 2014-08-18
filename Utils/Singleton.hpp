/**
    Example:

    class A { };

    SingletonVar(A, new A());
    A* GetInstance() { return SingletonVarName(A).Get(); }
**/

#pragma once

#ifndef _MR_SINGLETON_H_
#define _MR_SINGLETON_H_

namespace MR {

template< typename type >
class Singleton {
public:
    virtual ~Singleton() {}

    static type* GetInstance() {
        if(_singleton_instance == nullptr) _singleton_instance = new type();
        return _singleton_instance;
    }

    static void DestroyInstance() {
        if(_singleton_instance != nullptr) {
            delete _singleton_instance;
            _singleton_instance = nullptr;
        }
    }
private:
    static type * _singleton_instance;
};

template< typename type >
type* Singleton<type>::_singleton_instance = nullptr;

}

#endif // _MR_SINGLETON_H_
