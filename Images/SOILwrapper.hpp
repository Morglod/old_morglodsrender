#pragma once

#include "ImageInterfaces.hpp"

namespace mr {

class ImageLoaderSOIL : public IImageLoader {
public:
    IImagePtr Load(std::string const& path, Options const& options) override;
    IImagePtr Load(unsigned char* memory, size_t const& memSize, Options const& options) override;
    virtual ~ImageLoaderSOIL() {}
};

class ImageSaverSOIL : public IImageSaver {
public:
    bool Save(std::string const& path, Options const& options, const IImage* const image) override;
    virtual ~ImageSaverSOIL() {}
};

}
