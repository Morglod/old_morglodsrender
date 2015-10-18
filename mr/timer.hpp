#pragma once

#include <chrono>

namespace mr {

typedef std::chrono::system_clock SystemClockT;
typedef std::chrono::high_resolution_clock HighresClockT;

template<typename ClockT>
class Timer {
public:
    typedef std::chrono::time_point<ClockT> TimeT;

    inline void Start() {
        _begin = ClockT::now();
    }

    inline TimeT End() {
        _end = ClockT::now() - _begin;
    }

    inline TimeT Loop() {
        const auto r = End();
        Start();
        return r;
    }
private:
    TimeT _begin, _end;
};

}
