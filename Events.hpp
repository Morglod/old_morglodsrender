#pragma once

#ifndef _MR_EVENTS_H_
#define _MR_EVENTS_H_

#include <vector>
#include <algorithm>

namespace MR {
template<typename... Args>
class Event {
public:
    typedef void (*FuncP)(void* sender, Args... args);
protected:
    std::vector<FuncP> _funcs;
public:
    inline virtual void Add(FuncP f) {
        _funcs.push_back(f);
    }

    inline virtual void Sub(FuncP f) {
        auto it = std::find(_funcs.begin(), _funcs.end(), f);
        if(it == _funcs.end()) return;
        _funcs.erase(it);
    }

    inline virtual void SubAll() {
        _funcs.clear();
    }

    inline virtual void Call(void* sender, Args... args) const {
        if(_funcs.size() != 0){
            for(auto it = _funcs.begin(); it != _funcs.end(); ++it)
                (*it)(sender, args...);
        }
    }

    inline void operator() (void* s, Args... args) const {
        if(_funcs.size() != 0) Call(s, args...);
    }

    Event() {}

    virtual ~Event() {
        SubAll();
    }
};

template<typename... Args>
class TimerEvent : public Event<Args...> {
protected:
    float timerTickTime;
    float timerCurrentTime;
public:
    Event<TimerEvent*, float, Args...> OnUpdate;

    inline virtual void Update(float delta, void* s, Args... args) {
        OnUpdate(this, this, delta, args...);
        timerCurrentTime += delta;
        if(timerCurrentTime >= timerTickTime) {
            this->Call(s, args...);
            timerCurrentTime -= timerTickTime;
        }
    }

    inline virtual float GetCurrentTime() const {
        return timerCurrentTime;
    }

    inline virtual float GetTickTime() const {
        return timerTickTime;
    }

    inline virtual void SetCurrentTime(float t) {
        timerCurrentTime = t;
    }

    Event<TimerEvent*, float> OnNewTickTime;

    inline virtual void SetTickTime(float t) {
        OnNewTickTime(this, this, t);
        timerTickTime = t;
    }

    inline TimerEvent(float currentTime, float tickTime)
        : Event<Args...>(), timerTickTime(tickTime), timerCurrentTime(currentTime) {}
};

}
#endif
