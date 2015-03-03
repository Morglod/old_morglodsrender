#pragma once

#include "Buffers/BuffersInterfaces.hpp"
#include "Textures/TextureInterfaces.hpp"
#include "RTT/FrameBufferInterfaces.hpp"

#include <Containers.hpp>
#include <vector>
#include <unordered_map>

namespace mr {

class ITexture;
class ITextureSettings;
class IFrameBuffer;

class StateCache;
typedef std::shared_ptr<StateCache> StateCachePtr;
typedef std::weak_ptr<StateCache> StateCacheWeakPtr;

class StateCache {
public:
    void ResetCache();
    bool ReBindBuffers();
    bool ReBindUBOs();
    bool ReBindTransformFeedbacks();
    bool ReBindAll();

    bool BindBuffer(IGPUBuffer* buffer, IGPUBuffer::BindTarget const& target);
    bool BindUniformBuffer(IGPUBuffer* buffer, unsigned int const& index);
    bool BindTransformFeedbackBuffer(IGPUBuffer* buffer, unsigned int const& index);
    IGPUBuffer* GetBindedBuffer(IGPUBuffer::BindTarget const& target);
    IGPUBuffer* GetBindedUniformBuffer(unsigned int const& index);
    IGPUBuffer* GetBindedTransformFeedbackBuffer(unsigned int const& index);
    bool ReBindBuffer(IGPUBuffer* buffer, IGPUBuffer::BindTarget const& target, IGPUBuffer** was);

    virtual ~StateCache();

    static StateCache* GetDefault(); //this thread default, 0 index
    static StateCacheWeakPtr New();
    static mu::ArrayRef<StateCacheWeakPtr> GetAll();
    static mu::ArrayHandle<StateCacheWeakPtr> GetThisThread();
    static StateCacheWeakPtr GetThisThread(size_t const& index);

    static const unsigned int* GetGLBufferTargets();
private:
    StateCache();

    mu::ArrayHandle<IGPUBuffer*> _buffers;
    std::unordered_map<unsigned int, IGPUBuffer*> _ubos;
    std::unordered_map<unsigned int, IGPUBuffer*> _transformFeedbacks;
    mu::ArrayHandle<ITexture*> _textures;
    mu::ArrayHandle<ITextureSettings*> _textureSettings;
    IFrameBuffer* _framebuffer;
};

}
