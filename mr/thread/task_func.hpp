#pragma once

#include "future.hpp"
#include <functional>

namespace mr {

template<typename A>
class TaskFunc;

template<typename RetT, typename... ArgsT>
class TaskFunc<RetT(ArgsT...)> {
public:
    typedef Future<RetT> FutureT;
    typedef std::function<RetT (ArgsT...)> FuncT;

    FuncT func;
    FutureT ret;

    inline static void Call_(TaskFunc<RetT(ArgsT...)>* task, ArgsT... args) {
        task->ret.Set(task->func(args...));
    }

	inline void Call(ArgsT... args) {
		Call_(this, args...);
	}

	inline FutureT GetFuture() const {
        return ret;
	}

	inline FutureT get_future() const { //std::packaged_task compat
        return ret;
	}

	inline void operator () (ArgsT... args) {
		Call_(this, args...);
	}

	constexpr TaskFunc() : func(nullptr) {}
	constexpr TaskFunc(TaskFunc const& cpy) : func(cpy.func), ret(cpy.ret) {}
	constexpr TaskFunc(FuncT const& f) : func(f) {}
};

}
