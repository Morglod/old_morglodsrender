#include "StateCache.hpp"
#include "Utils/Log.hpp"
#include "Utils/Debug.hpp"
#include "MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

//without transform and uniform
#define MR_BUFFERS_BIND_TARGETS_NUM 12

unsigned int MR_BUFFER_TARGET[] {
    GL_ARRAY_BUFFER,
    GL_ATOMIC_COUNTER_BUFFER,
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER,
    GL_PIXEL_PACK_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_QUERY_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER
};

namespace mr {

std::vector<StateCacheWeakPtr> MR_STATE_CACHES;
thread_local std::vector<StateCachePtr> MR_STATE_CACHES_THIS_THREAD = std::vector<StateCachePtr>();

void StateCache::ResetCache() {
    _buffers = mu::ArrayHandle<IGPUBuffer*>(new IGPUBuffer*[MR_BUFFERS_BIND_TARGETS_NUM], MR_BUFFERS_BIND_TARGETS_NUM, true);
    for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i) _buffers.GetArray()[i] = nullptr;

    _ubos.clear();
    _transformFeedbacks.clear();
}

bool StateCache::ReBindBuffers() {
    auto bufAr = _buffers.GetArray();
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(size_t i = 0; i < MR_BUFFERS_BIND_TARGETS_NUM; ++i) {
            glBindBuffer(MR_BUFFER_TARGET[(size_t)i], (bufAr[i] == nullptr) ? 0 : bufAr[i]->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindUBOs() {
    if(_ubos.size() == 0) return true;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(auto& ubo_pair : _ubos) {
            glBindBufferBase(GL_UNIFORM_BUFFER, ubo_pair.first, (ubo_pair.second == nullptr) ? 0 : ubo_pair.second->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindTransformFeedbacks() {
    if(_transformFeedbacks.size() == 0) return true;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        for(auto& tf_pair : _transformFeedbacks) {
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, tf_pair.first, (tf_pair.second == nullptr) ? 0 : tf_pair.second->GetGPUHandle());
        },
        return false;
    )
    return true;
}

bool StateCache::ReBindAll() {
    return ReBindBuffers() && ReBindUBOs() && ReBindTransformFeedbacks();
}

bool StateCache::BindBuffer(IGPUBuffer* buffer, IGPUBuffer::BindTarget const& target) {
    if(target == IGPUBuffer::NotBinded) {
        mr::Log::LogString("Strange \"NotBinded\" target in StateCache::BindBuffer.", MR_LOG_LEVEL_WARNING);
        return false;
    }

    AssertAndExec((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM, return false);

    auto buffersArray = _buffers.GetArray();
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        buffersArray[(size_t)target] = nullptr;

        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBuffer(MR_BUFFER_TARGET[(size_t)target], 0); ,
            return false;
        )
        return true;
    }

    if(buffersArray[(size_t)target] == buffer) return true;

    buffersArray[(size_t)target] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBuffer(MR_BUFFER_TARGET[(size_t)target], buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

bool StateCache::BindUniformBuffer(IGPUBuffer* buffer, unsigned int const& index) {
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        _ubos[index] = nullptr;
        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBufferBase(GL_UNIFORM_BUFFER, index, 0); ,
            return false;
        )
        return true;
    }

    if(_ubos[index] == buffer) return true;

    _ubos[index] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

bool StateCache::BindTransformFeedbackBuffer(IGPUBuffer* buffer, unsigned int const& index) {
    if(buffer == nullptr || buffer == 0 || buffer->GetGPUHandle() == 0) {
        _transformFeedbacks[index] = nullptr;
        MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
            glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, 0); ,
            return false;
        )
        return true;
    }

    if(_transformFeedbacks[index] == buffer) return true;

    _transformFeedbacks[index] = buffer;
    MR_BUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, buffer->GetGPUHandle()); ,
        return false;
    )
    return true;
}

IGPUBuffer* StateCache::GetBindedBuffer(IGPUBuffer::BindTarget const& target) {
    AssertAndExec((size_t)target < MR_BUFFERS_BIND_TARGETS_NUM, return nullptr);
    return _buffers.GetArray()[(size_t)target];
}

IGPUBuffer* StateCache::GetBindedUniformBuffer(unsigned int const& index) {
    if(_ubos.count(index) == 0) return nullptr;
    return _ubos[index];
}

IGPUBuffer* StateCache::GetBindedTransformFeedbackBuffer(unsigned int const& index) {
    if(_transformFeedbacks.count(index) == 0) return nullptr;
    return _transformFeedbacks[index];
}

bool StateCache::ReBindBuffer(IGPUBuffer* buffer, IGPUBuffer::BindTarget const& target, IGPUBuffer** was) {
    IGPUBuffer* binded = GetBindedBuffer(target);
    if(binded == buffer) return true;
    *was = binded;
    if(BindBuffer(buffer, target)) return true;
    else {
        BindBuffer(*was, target);
        return false;
    }
}

StateCache::StateCache() {
}

StateCache::~StateCache() {
}

StateCacheWeakPtr StateCache::New() {
    StateCachePtr stateCache(new StateCache());
    stateCache->ResetCache();
    MR_STATE_CACHES_THIS_THREAD.push_back(stateCache);
    StateCacheWeakPtr weakPtr(stateCache);
    MR_STATE_CACHES.push_back(weakPtr);
    return weakPtr;
}

mu::ArrayRef<StateCacheWeakPtr> StateCache::GetAll() {
    return mu::ArrayRef<StateCacheWeakPtr>(MR_STATE_CACHES.data(), MR_STATE_CACHES.size());
}

mu::ArrayHandle<StateCacheWeakPtr> StateCache::GetThisThread() {
    mu::ArrayHandle<StateCacheWeakPtr> ar(new StateCacheWeakPtr[MR_STATE_CACHES_THIS_THREAD.size()], MR_STATE_CACHES_THIS_THREAD.size(), true);
    auto arPtr = ar.GetArray();
    for(size_t i = 0; i < MR_STATE_CACHES_THIS_THREAD.size(); ++i) arPtr[i] = StateCacheWeakPtr(MR_STATE_CACHES_THIS_THREAD[i]);
    return ar;
}

StateCacheWeakPtr StateCache::GetThisThread(size_t const& index) {
    if(index >= MR_STATE_CACHES_THIS_THREAD.size()) return StateCacheWeakPtr();
    return StateCacheWeakPtr(MR_STATE_CACHES_THIS_THREAD[index]);
}

StateCache* StateCache::GetDefault() {
    if(MR_STATE_CACHES_THIS_THREAD.empty()) New();
    return MR_STATE_CACHES_THIS_THREAD[0].get();
}

const unsigned int * StateCache::GetGLBufferTargets() {
    return MR_BUFFER_TARGET;
}

}
