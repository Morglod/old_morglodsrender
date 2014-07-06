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

template< typename oclass >
class ISingletonObject {
public:
    oclass * instance;
    virtual oclass * Get() = 0;
    virtual void Destroy() { if(instance != 0) { delete instance; instance = 0; } }
    virtual ~ISingletonObject() { Destroy(); }
};
}

#define __MR_CONCAT2(x,y) x##y
#define __MR_CONCAT(x,y) __MR_CONCAT2(x, y)

#define SingletonName(ObjectType) __MR_CONCAT(AutoSingleton_, ObjectType)

#define MakeSingleton(ObjectType, CreateNewObjectCode) \
class SingletonName(ObjectType) : public ISingletonObject<ObjectType> { \
public: \
    ObjectType * Get() override { \
        if(instance == 0) instance = CreateNewObjectCode; \
        return instance; \
    } \
    virtual ~SingletonName(ObjectType)() {} \
};

#define SingletonVarI(ObjectType, CreateNewObjectCode, x) \
MakeSingleton(ObjectType, CreateNewObjectCode) \
SingletonName(ObjectType) __MR_CONCAT(__MR_CONCAT(__SINGLETON__, SingletonName(ObjectType)), x)

#define SingletonVar(ObjectType, CreateNewObjectCode) SingletonVarI(ObjectType, CreateNewObjectCode, 0)

#define SingletonVarNameI(ObjectType, x) __MR_CONCAT(__MR_CONCAT(__SINGLETON__, SingletonName(ObjectType)), x)
#define SingletonVarName(ObjectType) SingletonVarNameI(ObjectType, 0)

#endif // _MR_SINGLETON_H_
