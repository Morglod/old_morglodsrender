#include "mr/thread/semaphore.hpp"

namespace mr {

void Semaphore::Notify() {
    std::unique_lock<std::mutex> lock(mtx);
    ++counter;
    cond.notify_one();
}

void Semaphore::Wait() {
    std::unique_lock<std::mutex> lock(mtx);
    while(!counter)
        cond.wait(lock);
    --counter;
}

}
