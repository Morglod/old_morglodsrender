#pragma once

#include "build.hpp"

#include <functional>
#include <future>

namespace mr {

class MR_API ContextMgr {
public:
    virtual void MakeCurrent(void* context) = 0;
    virtual void* GetMainContext() = 0;
    virtual ~ContextMgr() {}
};

class MR_API Core {
public:
    typedef std::packaged_task<void (void* arg)> PackagedTaskT;
    typedef std::function<void (void* arg)> ExecTaskT;
    typedef std::future<void> TaskReturnT;

    static bool Init(ContextMgr* context);
    static void Shutdown();
    static TaskReturnT Exec(ExecTaskT const& func, void* arg = nullptr);
    static TaskReturnT Exec(PackagedTaskT task, void* arg = nullptr);
};

}
