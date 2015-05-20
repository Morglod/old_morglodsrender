#include "RTTManager.hpp"
#include "FrameBuffer.hpp"
#include "RenderBuffer.hpp"

#include "../Utils/Log.hpp"

namespace mr {

template<>
mr::RTTManager mu::StaticSingleton<mr::RTTManager>::_singleton_instance = mr::RTTManager();

FrameBuffer* RTTManager::CreateFrameBuffer() {
    FrameBuffer* fb = new FrameBuffer();
    if(!fb->Create()) {
        mr::Log::LogString("Failed RTTManager::CreateFrameBuffer. Failed create frame buffer.", MR_LOG_LEVEL_ERROR);
        delete fb;
        return nullptr;
    }
    TGPUObjectManager<FrameBuffer>::_Register(fb);
    return fb;
}

RenderBuffer* RTTManager::CreateRenderBuffer(Texture::StorageDataFormat const& sdf,
                                     glm::uvec2 const& size,
                                     int const& samples /*multisample*/ )
{
    RenderBuffer* rb = new RenderBuffer();
    if(!rb->Create(sdf, size, samples)) {
        mr::Log::LogString("Failed RTTManager::CreateRenderBuffer. Failed create render buffer.", MR_LOG_LEVEL_ERROR);
        delete rb;
        return nullptr;
    }
    TGPUObjectManager<RenderBuffer>::_Register(rb);
    return rb;
}

RTTManager::RTTManager() {
}

RTTManager::~RTTManager() {
    DestroyAllFrameBuffers();
    DestroyAllRenderBuffers();
}

}
