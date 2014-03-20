/** HEADER ONLY **/

#pragma once

#ifndef _MR_EVENTS_H_
#define _MR_EVENTS_H_

#include "Types.hpp"

#include <vector>
#include <algorithm>

namespace MR {

template<typename... Args>
class Event : public Super {
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
            for(auto it = _funcs.begin(); it != _funcs.end(); ++it)
                (*it)(sender, args...);
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

}
#endif
