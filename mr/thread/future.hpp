#pragma once

#include "../build.hpp"

#include <condition_variable>
#include <mutex>

namespace mr {

template<typename T>
class Future {
public:

    inline bool Wait() {
    #if MR_MULTITHREAD_FUTURE == 1
        std::unique_lock<std::mutex> lock(_dat->_mtx);
        if(_dat->_set) return _dat->_error;
        _dat->_cond.wait(lock);
    #endif
        return _dat->_error;
    }

    inline bool wait() { //std compat.
        return Wait();
    }

    inline void Set(T const& value_) {
    #if MR_MULTITHREAD_FUTURE == 1
        std::unique_lock<std::mutex> lock(_dat->_mtx);
        _dat->_value = value_;
        _dat->_set = true;
        _dat->_cond.notify_all();
    #else
        _dat->_value = value_;
        _dat->_set = true;
    #endif
    }

    inline void SetError() {
    #if MR_MULTITHREAD_FUTURE == 1
        std::unique_lock<std::mutex> lock(_dat->_mtx);
        _dat->_error = true;
        _dat->_set = true;
        _dat->_cond.notify_all();
    #else
        _dat->_error = true;
        _dat->_set = true;
    #endif
    }

    inline T GetValue() const {
        return _dat->_value;
    }

    inline bool IsError() const {
        return _dat->_error;
    }

    inline bool IsSet() const {
        return _dat->_set;
    }

    constexpr Future() : _dat(new Dat()) {}
    constexpr Future(Future<T> const& cpy) : _dat(cpy._dat) {}
    inline Future(T const& value) : Future() {
        _dat->_value = value;
        _dat->_set = true;
    }
private:
    struct Dat {
        std::mutex _mtx;
        std::condition_variable _cond;
        T _value;
        bool _error = false;
        bool _set = false;
    };
    std::shared_ptr<Dat> _dat;
};

}
