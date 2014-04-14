/**

    !! C++11 REQUIRED !!

**/

#pragma once

#ifndef _MR_TIME_H_
#define _MR_TIME_H_

#include <chrono>
#include <string>

namespace MR{

class Time {
public:
    typedef std::chrono::hours Hours;
    typedef std::chrono::minutes Minutes;
    typedef std::chrono::seconds Seconds;
    typedef std::chrono::milliseconds Milliseconds;
    typedef std::chrono::microseconds Microseconds;
    typedef std::chrono::nanoseconds Nanoseconds;

    typedef std::chrono::high_resolution_clock HighResolutionClock;
    typedef std::chrono::system_clock SystemClock;
    typedef std::chrono::steady_clock SteadyClock;
};

/**
ClockClass is one of
    Time::HighResolutionClock, Time::SystemClock, Time::SteadyClock

DurotationType is one of
    Time::Hours, Time::Minutes, Time::Seconds,
    Time::Milliseconds, Time::Microseconds, Time::Nanoseconds

Example:
    MR::Timer<MR::Time::HighResolutionClock, MR::Time::Nanoseconds> timer;

    timer.Start();
    time_test();
    timer.Stop();

    std::cout << timer.TimerTime().count() << std::endl;
**/
template<typename ClockClass, typename DurotationType>
class Timer {
public:
    typedef typename ClockClass::time_point time_point;

    void Start() { tbegin = ClockClass::now(); }
    time_point Stop() { tend = ClockClass::now(); return tend; }
    time_point Begin() { return tbegin; }
    time_point End() { return tend; }
    DurotationType Elapsed() { return std::chrono::duration_cast<DurotationType>(ClockClass::now() - tbegin); }
    DurotationType TimerTime() { return std::chrono::duration_cast<DurotationType>(tend - tbegin); }
private:
    time_point tbegin;
    time_point tend;
};

}

#endif // _MR_TIME_H_
