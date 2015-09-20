#pragma once

#include "../Textures/Texture.hpp"

#include "../pre_glm.hpp"

namespace mr {

class ISceneManager;
class ICamera;
class FrameBuffer;

class ShadowMap {
public:
    struct CreationParams {
        glm::uvec2 size = glm::uvec2(512,512);
        Texture::StorageDataFormat sdf = Texture::SDF_DEPTH_COMPONENT24;
    };

    virtual bool Init(CreationParams const& params);
    virtual bool Capture(ISceneManager* scene, ICamera* view);

    virtual void Destroy();

    ShadowMap() = default;
    virtual ~ShadowMap() = default;
protected:
    FrameBuffer* _fb = nullptr;
    Texture* _depthTex = nullptr;
};

}
