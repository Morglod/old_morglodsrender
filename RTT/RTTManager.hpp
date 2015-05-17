#pragma once

#include "../Textures/Texture.hpp"
#include <mu/Singleton.hpp>

#include <unordered_set>

namespace mr {

class FrameBuffer;
class RenderBuffer;

class RTTManager : public mu::StaticSingleton<RTTManager> {
public:
    FrameBuffer* CreateFrameBuffer();
    RenderBuffer* CreateRenderBuffer(Texture::StorageDataFormat const& sdf,
                                     glm::uvec2 const& size,
                                     int const& samples /*multisample*/ );

    void DestroyAllFrameBuffers();
    void DestroyAllRenderBuffers();

    RTTManager();
    virtual ~RTTManager();
protected:
    void _RegisterFrameBuffer(FrameBuffer* fb);
    void _UnRegisterFrameBuffer(FrameBuffer* fb);
    void _RegisterRenderBuffer(RenderBuffer* fb);
    void _UnRegisterRenderBuffer(RenderBuffer* fb);

    std::unordered_set<FrameBuffer*> _fbs;
    std::unordered_set<RenderBuffer*> _rbs;
};

}
