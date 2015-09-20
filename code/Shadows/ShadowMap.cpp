#include "ShadowMap.hpp"
#include "../RTT/RTTManager.hpp"
#include "../Textures/TextureManager.hpp"
#include "../RTT/FrameBuffer.hpp"

#include "../Scene/SceneManagerInterfaces.hpp"

namespace mr {

bool ShadowMap::Init(CreationParams const& params) {
    TextureManager& tex = TextureManager::GetInstance();

    Texture::CreationParams texParams;
    texParams.wrapS = TextureSettings::Wrap_CLAMP;
    texParams.wrapT = TextureSettings::Wrap_CLAMP;
    texParams.wrapR = TextureSettings::Wrap_CLAMP;

    _depthTex = tex.CreateTexture2D(params.size, Texture::DT_UNSIGNED_INT, Texture::DF_RED, params.sdf, texParams);

    RTTManager& rtt = RTTManager::GetInstance();

    _fb = rtt.CreateFrameBuffer();
    _fb->AttachDepth(_depthTex);
    if(_fb->Complete() != FrameBuffer::Success) {
        Destroy();
        return false;
    }

    return true;
}

bool ShadowMap::Capture(ISceneManager* scene, ICamera* view) {
    return true;
}

void ShadowMap::Destroy() {
    TextureManager& tex = TextureManager::GetInstance();
    RTTManager& rtt = RTTManager::GetInstance();

    rtt.DestroyFrameBuffer(_fb);
    tex.Destroy(_depthTex);
}

}
