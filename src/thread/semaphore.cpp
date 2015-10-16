#include "mr/thread/semaphore.hpp"

namespace mr {

void Semaphore::Notify() {
#if MR_MULTITHREAD_SEMAPHORE == 1
    std::unique_lock<std::mutex> lock(mtx);
    ++counter;
    cond.notify_one();
#else
    ++counter;
#endif
}

bool Semaphore::Wait() {
#if MR_MULTITHREAD_SEMAPHORE == 1
    std::unique_lock<std::mutex> lock(mtx);
    while(!counter)
        cond.wait(lock);
    --counter;
    return true;
#else
    if(counter == 0) return false;
    --counter;
    return true;
#endif
}

}
