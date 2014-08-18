#pragma once

#ifndef _MR_WORKER_H_
#define _MR_WORKER_H_

#include "Containers.hpp"

namespace MR {

class Worker {
public:
    class WorkerCommand {
    public:
        virtual void Do() = 0;
        virtual ~WorkerCommand() {}
    };

    virtual void Add(const WorkerCommand& cmd) {
        _queue.Push(cmd);
    }

    virtual bool Get(WorkerCommand& cmd) {
        return _queue.Pop(cmd);
    }

    virtual bool IsAny() {
        return _queue.IsEmpty();
    }

    Worker() : _queue() {}
    virtual ~Worker() {}
protected:
    MR::ThreadSafeQueue<WorkerCommand> _queue;
};

}

#endif // _MR_WORKER_H_
