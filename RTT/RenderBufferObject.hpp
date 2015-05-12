#pragma once

#include "RenderBufferInterfaces.hpp"

namespace mr {

class RenderBuffer : public IRenderBuffer {
public:
    bool Create(mr::Texture::StorageDataFormat const& storageDataFormat, unsigned int const& width, unsigned int const& height, unsigned int const& samples = 0) override;
    void Bind() override;

    void Destroy() override;

    RenderBuffer();
    virtual ~RenderBuffer();
};

}
