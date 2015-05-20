#pragma once

#include "../Textures/Texture.hpp"
#include "RenderBuffer.hpp"
#include "../GPUObjectManager.hpp"
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

    inline void DeleteFrameBuffer(FrameBuffer*& what) { this->TGPUObjectManager<FrameBuffer>::Delete(what); }
    inline void DeleteRenderBuffer(RenderBuffer*& what) { this->TGPUObjectManager<RenderBuffer>::Delete(what); }

    RTTManager();
    virtual ~RTTManager();
};

}
