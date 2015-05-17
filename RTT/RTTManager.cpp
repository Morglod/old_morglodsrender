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
    _RegisterFrameBuffer(fb);
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
    _RegisterRenderBuffer(rb);
    return rb;
}

void RTTManager::DestroyAllFrameBuffers() {
    for(FrameBuffer* fb : _fbs) fb->Destroy();
    _fbs.clear();
}

void RTTManager::DestroyAllRenderBuffers() {
    for(RenderBuffer* rb : _rbs) rb->Destroy();
    _rbs.clear();
}

void RTTManager::_RegisterFrameBuffer(FrameBuffer* tex) {
    _fbs.insert(tex);
}

void RTTManager::_UnRegisterFrameBuffer(FrameBuffer* tex) {
    _fbs.erase(tex);
}

void RTTManager::_RegisterRenderBuffer(RenderBuffer* rb) {
    _rbs.insert(rb);
}

void RTTManager::_UnRegisterRenderBuffer(RenderBuffer* rb) {
    _rbs.erase(rb);
}

RTTManager::RTTManager() {
}

RTTManager::~RTTManager() {
    DestroyAllFrameBuffers();
    DestroyAllRenderBuffers();
}

}
