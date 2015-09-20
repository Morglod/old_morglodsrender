#pragma once

#include <unordered_set>

namespace mr {

template<typename T>
class TGPUObjectManager {
public:
    inline size_t GetUsedGPUMemory() { return _usedMem; }

    void Destroy(T*& what) {
        _Destroy(what);
        what = nullptr;
    }

    void DestroyAll() {
        while(!_objects.empty()) _Destroy(*(_objects.begin()));
    }

    virtual ~TGPUObjectManager() {
        DestroyAll();
    }

protected:
    void _Register(T* what) {
        _objects.insert(what);
        _usedMem += what->GetGPUMem();
    }

    void _UnRegister(T* what) {
        _objects.erase(what);
        _usedMem -= what->GetGPUMem();
    }

    void _GetObjectsSetTo(std::unordered_set<T*>*& where) {
        where = &_objects;
    }
    size_t _usedMem = 0;

private:
    void _Destroy(T* what) {
        if(what == nullptr) return;
        _UnRegister(what);
        what->Destroy();
        delete what;
    }

    std::unordered_set<T*> _objects;
};

}
