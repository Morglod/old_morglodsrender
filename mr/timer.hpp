#pragma once

#include <chrono>

namespace mr {

typedef std::chrono::system_clock SystemClockT;
typedef std::chrono::high_resolution_clock HighresClockT;

typedef std::chrono::nanoseconds NanoTimeT;
typedef std::chrono::microseconds MicroTimeT;
typedef std::chrono::milliseconds MilliTimeT;
typedef std::chrono::seconds SecTimeT;
typedef std::chrono::minutes MinTimeT;
typedef std::chrono::hours HourTimeT;

template<typename ClockT, typename TimeT>
class Timer {
public:
    typedef typename TimeT::rep RetT;

    inline void Start() {
        _begin = ClockT::now();
    }

    inline RetT End() {
        return std::chrono::duration_cast<TimeT>(ClockT::now() - _begin).count();
    }

    inline RetT Loop() {
        const auto r = End();
        Start();
        return r;
    }
private:
    std::chrono::time_point<ClockT> _begin;
};

}
