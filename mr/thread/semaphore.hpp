#pragma once

#include "../build.hpp"

#include <thread>
#include <condition_variable>
#include <mutex>

namespace mr {

class MR_API Semaphore {
public:
    void Notify();
    bool Wait(); // return true, if ok. false on error while waiting (or counter == 0 in one thread build)

private:
    std::mutex mtx;
    std::condition_variable cond;
    uint64_t counter = 0;
};

}

