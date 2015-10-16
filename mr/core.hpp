#pragma once

#include "build.hpp"
#include "thread/task_func.hpp"

namespace mr {

class MR_API ContextMgr {
public:
    virtual void MakeCurrent(void* context) = 0;
    virtual void* GetMainContext() = 0;
    virtual void Swap() = 0;
    virtual ~ContextMgr() {}
};

class MR_API Core {
public:
    typedef TaskFunc<uint8_t (void* arg)> TaskFuncT;
    typedef std::function<uint8_t (void* arg)> FuncT;
    typedef Future<uint8_t> TaskReturnT;

    static bool Init(ContextMgr* context);
    static void Shutdown();
    static TaskReturnT Swap();
    static TaskReturnT Exec(FuncT const& func, void* arg = nullptr);
    static TaskReturnT Exec(TaskFuncT task, void* arg = nullptr);
    static bool IsWorkerThread();
};

}
