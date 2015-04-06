#pragma once

#include <Containers.hpp>
#include <vector>
#include <unordered_map>

namespace mr {

class IGPUBuffer;
class ITexture;
class ITextureSettings;
class IFrameBuffer;

class StateCache;
typedef std::shared_ptr<StateCache> StateCachePtr;
typedef std::weak_ptr<StateCache> StateCacheWeakPtr;

class StateCache {
public:
    enum BufferBindTarget : unsigned char {
        ArrayBuffer = 0,
        AtomicCounterBuffer = 1,
        CopyReadBuffer,
        CopyWriteBuffer,
        DrawIndirectBuffer,
        DispatchIndirectBuffer,
        ElementArrayBuffer,
        PixelPackBuffer,
        PixelUnpackBuffer,
        QueryBuffer,
        ShaderStorageBuffer,
        TextureBuffer,
        TransformFeedbackBuffer,
        UniformBuffer,

        NotBinded = 255
    };

    void ResetCache();
    bool ReBindBuffers();
    bool ReBindUBOs();
    bool ReBindTransformFeedbacks();
	bool ReBindTextures();
	bool ReBindFrameBuffers();
    bool ReBindAll();

    bool BindBuffer(IGPUBuffer* buffer, BufferBindTarget const& target);
    bool BindUniformBuffer(IGPUBuffer* buffer, unsigned int const& index);
    bool BindTransformFeedbackBuffer(IGPUBuffer* buffer, unsigned int const& index);
    IGPUBuffer* GetBindedBuffer(BufferBindTarget const& target);
    IGPUBuffer* GetBindedUniformBuffer(unsigned int const& index);
    IGPUBuffer* GetBindedTransformFeedbackBuffer(unsigned int const& index);
    bool ReBindBuffer(IGPUBuffer* __restrict__ buffer, BufferBindTarget const& target, IGPUBuffer** __restrict__ was);

    bool BindTexture(ITexture* texture, unsigned int const& unit);
    ITexture* GetBindedTexture(unsigned int const& unit);
    ITextureSettings* GetBindedTextureSettings(unsigned int const& unit);
    bool ReBindTexture(ITexture* __restrict__ texture, unsigned int const& unit, ITexture** __restrict__ was);
    bool GetFreeTextureUnit(unsigned int& freeUnit);

	bool BindFramebuffer(IFrameBuffer* frameBuffer);
	IFrameBuffer* GetBindedFramebuffer();
	bool ReBindFramebuffer(IFrameBuffer* __restrict__ frameBuffer, IFrameBuffer** __restrict__ was);

    virtual ~StateCache();

    static StateCache* GetDefault(); //this thread default, 0 index
    static StateCacheWeakPtr New();
    static mu::ArrayRef<StateCacheWeakPtr> GetAll();
    static mu::ArrayHandle<StateCacheWeakPtr> GetThisThread();
    static StateCacheWeakPtr GetThisThread(size_t const& index);
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
