#pragma once

namespace mr {

template<typename T>
struct PromiseData {
    std::promise<T> promise;
    typedef std::shared_ptr<PromiseData<T>> Ptr;

    PromiseData() : promise() {}
};

}
