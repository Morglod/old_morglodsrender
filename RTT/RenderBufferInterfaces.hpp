#pragma once

#include "../CoreObjects.hpp"
#include "../Textures/TextureInterfaces.hpp"

namespace mr {

class IRenderBuffer : public IGPUObjectHandle {
public:
    //Pass samples as non zero to use multisampling
    virtual bool Create(mr::ITexture::StorageDataFormat const& storageDataFormat, unsigned int const& width, unsigned int const& height, unsigned int const& samples = 0) = 0;
    virtual void Bind() = 0;
};

}
