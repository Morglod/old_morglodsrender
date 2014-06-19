/** HEADER ONLY **/

#pragma once

#ifndef _MR_EVENTS_H_
#define _MR_EVENTS_H_

#include "Types.hpp"

#include <vector>
#include <algorithm>

namespace MR {

template<typename... Args>
class Event {
public:
    typedef void (*FuncP)(void* sender, Args... args);

    inline virtual void Add(const FuncP& f) { _funcs.push_back(f); }

    inline virtual void Sub(const FuncP& f) {
        auto it = std::find(_funcs.begin(), _funcs.end(), f);
        if(it == _funcs.end()) return;
        _funcs.erase(it);
    }
    inline virtual void SubAll() { _funcs.clear(); }

    inline virtual void Call(void* sender, Args... args) const {
        if(_funcs.size() != 0){
            for(size_t i = 0; i < _funcs.size(); ++i) {
                if(_funcs[i]) _funcs[i](sender, args...);
            }
        }
    }

    inline void operator() (void* s, Args... args) const { Call(s, args...); }
    inline void operator += (const FuncP& f){ Add(f); }
    inline void operator -= (const FuncP& f){ Sub(f); }

    Event() {}
    virtual ~Event() { SubAll(); }

protected:
    std::vector<FuncP> _funcs;
};

template<typename... Args>
class TimerEvent : public Event<Args...> {
public:
    Event<const float&, Args...> OnUpdate;
    Event<const float&> OnTickTimeChanged;

    inline virtual void Update(const float& delta, void* s, Args... args) {
        OnUpdate(this, delta, args...);
        timerCurrentTime += delta;
        if(timerCurrentTime >= timerTickTime) {
            this->Call(s, args...);
            timerCurrentTime -= timerTickTime;
        }
    }

    inline virtual float GetCurrentTime() const { return timerCurrentTime; }
    inline virtual float GetTickTime() const { return timerTickTime; }
    inline virtual void SetCurrentTime(const float& t) { timerCurrentTime = t; }

    inline virtual void SetTickTime(const float& t) {
        if(timerTickTime != t){
            timerTickTime = t;
            OnTickTimeChanged(this, t);
        }
    }

    TimerEvent(float currentTime, float tickTime) : Event<Args...>(), timerTickTime(tickTime), timerCurrentTime(currentTime) {}
    virtual ~TimerEvent() {}

protected:
    float timerTickTime;
    float timerCurrentTime;
};

template<typename... Args>
class EventListener;

template<typename... Args>
class EventHandle {
public:
    virtual void Invoke(EventListener<Args...>*, Args...) {}
    virtual ~EventHandle() {}
};

template<typename... Args>
class EventListener {
public:
    inline virtual void RegisterHandle(EventHandle<Args...>* h) {
        _handles.push_back(h);
    }

    inline virtual void UnRegisterHandle(EventHandle<Args...>* h){
        auto it = std::find(_handles.begin(), _handles.end(), h);
        if(it == _handles.end()) return;
        _handles.erase(it);
    }

    template<typename HandleClass, typename... ConstructArgs>
    inline HandleClass* CreateHandle(ConstructArgs... args){
        HandleClass* h = new HandleClass(args...);
        RegisterHandle(h);
        return h;
    }

    template<typename HandleClass>
    inline void DeleteHandle(HandleClass* h){
        auto it = std::find(_handles.begin(), _handles.end(), h);
        if(it == _handles.end()) return;
        _handles.erase(it);
        delete h;
    }

    inline virtual void Call(Args... args) const {
        if(_handles.size() != 0){
            for(size_t i = 0; i < _handles.size(); ++i) {
                _handles[i]->Invoke((EventListener<Args...>*)this, args...);
            }
        }
    }

    inline void operator() (Args... args) const { Call(args...); }

    bool freePermissions;

    EventListener() : freePermissions(true) { }
    virtual ~EventListener(){
        if(freePermissions){
            while(_handles.size() > 0){
                delete _handles.back();
                _handles.pop_back();
            }
        }
    }

protected:
    std::vector<EventHandle<Args...>*> _handles;
};

}
#endif
