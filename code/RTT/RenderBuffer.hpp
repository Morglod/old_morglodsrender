#pragma once

#include "../CoreObjects.hpp"
#include "../Textures/Texture.hpp"

namespace mr {

class RenderBuffer : public IGPUObjectHandle {
    friend class RTTManager;
public:
    void Destroy() override;

    virtual ~RenderBuffer();
protected:
    virtual bool Create(Texture::StorageDataFormat const& sdf,
                        glm::uvec2 const& size,
                        int const& samples /*multisample*/ );
    RenderBuffer() = default;
};

}
