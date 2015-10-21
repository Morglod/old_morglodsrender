#pragma once

/// MP_ENABLED
#define MP_ENABLED

#include <stack>
#include <string>
#include <chrono>

#include "mr/timer.hpp"

// "#define MR_PROFILE" build only
#ifdef MP_ENABLED
#ifndef MR_PROFILE
#undef MP_ENABLED
#endif
#endif

#ifdef MP_ENABLED

#define MP_BeginSampleT(_name_) \
{ \
	auto smpl = mp::Sample(); \
	smpl.name = std::string(_name_); \
	smpl.timer.Start(); \
	mp::RefSamples().push(smpl); \
}

#define MP_BeginSample(x) \
MP_BeginSampleT(#x)

#define MP_BeginFuncSample() \
MP_BeginSample( (__func__) )

#define MP_EndSample() \
{ \
    if(!mp::RefSamples().empty()) { \
        auto smpl = mp::RefSamples().top(); \
        mp::RefSamples().pop(); \
        smpl.time = smpl.timer.End(); \
        std::string parentName; \
        if(!mp::RefSamples().empty()) parentName = mp::RefSamples().top().name; \
        mp::Log(smpl, parentName); \
    } \
}

#define MP_ScopeSampleT(_name_) \
auto __scope_sample = mp::ScopeSample(_name_);

#define MP_ScopeSample(x) \
MP_ScopeSampleT(#x)

#define MP_ScopeFuncSample() \
auto __scope_sample mp::ScopeSample( (__func__) );

#else

#define MP_BeginSample(x)
#define MP_BeginSampleT(_name_)
#define MP_BeginFuncSample()
#define MP_EndSample()
#define MP_ScopeSample(x)
#define MP_ScopeSampleT(_name_)
#define MP_ScopeFuncSample()

#endif // MP_ENABLED

namespace mp {

typedef mr::Timer<mr::HighresClockT, mr::MilliTimeT> TimerT;
typedef TimerT::RetT TimeT;

struct Sample {
	TimerT timer;
	TimeT time;
	std::string name;
};

bool Init();
void Shutdown();

void Log(Sample const&, std::string const& parentName);

std::stack<Sample>& RefSamples();

struct ScopeSample {
	inline ScopeSample(const char* name) {
	    (void)name;
		MP_BeginSampleT(name);
	}
	inline ~ScopeSample() {
		MP_EndSample();
	}
};

}
