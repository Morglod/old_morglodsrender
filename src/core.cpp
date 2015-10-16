#include "mr/core.hpp"

#include "mr/pre/glew.hpp"
#include "mr/log.hpp"
#include "mr/info.hpp"

#include "mr/thread/semaphore.hpp"

#include <thread>
#include <deque>
#include <future>

namespace {

thread_local bool _core_worker_thread = false;

}

namespace mr {

class CoreImpl {
public:
    struct _Task {
        Core::TaskFuncT task;
        void* arg;

        _Task(Core::TaskFuncT&& t, void* a) : task(std::move(t)), arg(a) {}
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
        _core_worker_thread = true;

        glewExperimental = true;
        GLenum result = glewInit();
        if(result != GLEW_OK) {
            MR_LOG_ERROR(Core::Init, "glew initialization failed "+std::string((char*)glewGetErrorString(result)));
            self->initStatus = false;
        }

        if(Info::GetVersion() == Info::GLVersion::VNotSupported) {
            MR_LOG_WARNING(Core::Init, "Current opengl version (\""+Info::GetVersionAsString()+"\") is not supported. Try to use OpenGL 4.0 features");
        }

        glEnable (GL_DEPTH_TEST);
        glDepthFunc (GL_LESS);

        GLuint VertexArrayID;
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

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

bool Core::IsWorkerThread() {
    return _core_worker_thread;
}

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

Core::TaskReturnT Core::Swap() {
    if(Core::IsWorkerThread()) {
        coreImpl.ctx->Swap();
        return Core::TaskReturnT();
    } else {
        return Exec([](void*) -> uint8_t { coreImpl.ctx->Swap(); return 0; });
    }
}

Core::TaskReturnT Core::Exec(Core::FuncT const& func, void* arg) {
    return Exec(TaskFuncT(func), arg);
}

Core::TaskReturnT Core::Exec(TaskFuncT task, void* arg) {
    auto fu = task.get_future();
    coreImpl.exec_queue.push_back(CoreImpl::_Task(std::move(task), arg));
    coreImpl.semaphore.Notify();
    return fu;
}

}
