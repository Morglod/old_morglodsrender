#pragma once

#include "../Textures/Texture.hpp"
#include "RenderBuffer.hpp"
#include "../TGPUObjectManager.hpp"
#include <mu/Singleton.hpp>

#include <unordered_set>

namespace mr {

class FrameBuffer;
class RenderBuffer;

class RTTManager : public TGPUObjectManager<FrameBuffer>, public TGPUObjectManager<RenderBuffer>, public mu::StaticSingleton<RTTManager> {
public:
    FrameBuffer* CreateFrameBuffer();
    RenderBuffer* CreateRenderBuffer(Texture::StorageDataFormat const& sdf,
                                     glm::uvec2 const& size,
                                     int const& samples /*multisample*/ );

    inline void DestroyAllFrameBuffers() { TGPUObjectManager<FrameBuffer>::DestroyAll(); }
    inline void DestroyAllRenderBuffers() { TGPUObjectManager<RenderBuffer>::DestroyAll(); }

    inline void DestroyFrameBuffer(FrameBuffer*& what) { this->TGPUObjectManager<FrameBuffer>::Destroy(what); }
    inline void DestroyRenderBuffer(RenderBuffer*& what) { this->TGPUObjectManager<RenderBuffer>::Destroy(what); }

    RTTManager();
    virtual ~RTTManager();
};

}
