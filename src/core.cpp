#include "../mr/core.hpp"

#include "../mr/pre/glew.hpp"
#include "../mr/log.hpp"
#include "../mr/info.hpp"

#include "../mr/threads/semaphore.hpp"

#include <thread>
#include <deque>
#include <future>

namespace mr {

class CoreImpl {
public:
    struct _Task {
        Core::PackagedTaskT task;
        void* arg;

        _Task(Core::PackagedTaskT&& t, void* a) : task(std::move(t)), arg(a) {}
    };

    std::thread thread;
    mr::Semaphore semaphore;
    std::mutex critMtx; // startup and shutdown
    bool initStatus;
    ContextMgr* ctx;
    bool workerLoop = true;
    std::deque<_Task> exec_queue;

    static void Worker(CoreImpl* self) {
        auto mainCtx = self->ctx->GetMainContext();
        self->ctx->MakeCurrent(mainCtx);

        glewExperimental = true;
        GLenum result = glewInit();
        if(result != GLEW_OK) {
            MR_LOG_ERROR(Core::Init, "glew initialization failed "+std::string((char*)glewGetErrorString(result)));
            self->initStatus = false;
        }

        if(Info::GetVersion() == Info::GLVersion::VNotSupported) {
            MR_LOG_WARNING(Core::Init, "Current opengl version (\""+Info::GetVersionAsString()+"\") is not supported. Try to use OpenGL 4.0 features");
        }
        self->initStatus = true;
        self->critMtx.unlock();

        while(self->workerLoop) {
            self->semaphore.Wait();
            if(!self->workerLoop) break;
            // process callbacks
            while(!self->exec_queue.empty()) {
                auto& front = self->exec_queue.front();
                front.task(front.arg);
                self->exec_queue.pop_front();
            }
        }

        self->critMtx.unlock();
    }
};

CoreImpl coreImpl;

bool Core::Init(ContextMgr* context) {
    coreImpl.ctx = context;
    coreImpl.thread = std::thread(CoreImpl::Worker, &coreImpl);
    coreImpl.critMtx.lock();
    coreImpl.critMtx.lock();

    return coreImpl.initStatus;
}

void Core::Shutdown() {
    coreImpl.workerLoop = false;
    coreImpl.semaphore.Notify();
    coreImpl.critMtx.lock();
    coreImpl.thread.join();
}

Core::TaskReturnT Core::Exec(Core::ExecTaskT const& func, void* arg) {
    return Exec(PackagedTaskT(func), arg);
}

Core::TaskReturnT Core::Exec(PackagedTaskT task, void* arg) {
    auto fu = task.get_future();
    coreImpl.exec_queue.push_back(CoreImpl::_Task(std::move(task), arg));
    coreImpl.semaphore.Notify();
    return fu;
}

}
