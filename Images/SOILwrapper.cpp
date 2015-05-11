#include "SOILwrapper.hpp"
#include "Image.hpp"

#include "../Utils/Log.hpp"

#include "../Types.hpp"

#include <SOIL.h>

namespace mr {

IImagePtr ImageLoaderSOIL::Load(std::string const& path, Options const& options) {
    int width, height, channels, channels_flag;

    if(options.greyscale && options.alpha) channels_flag = SOIL_LOAD_LA;
    else if(!options.greyscale && options.alpha) channels_flag = SOIL_LOAD_RGBA;
    else if(options.greyscale && !options.alpha) channels_flag = SOIL_LOAD_L;
    else if(!options.greyscale && !options.alpha) channels_flag = SOIL_LOAD_RGB;

    unsigned char* data = SOIL_load_image(path.c_str(), &width, &height, &channels, channels_flag);

    if(data == 0) {
        mr::Log::LogString("Failed ImageLoaderSOIL::Load(). SOIL_load_image failed. SOIL: \"" + std::string(SOIL_last_result()) + "\"", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    if(width == 0) {
        mr::Log::LogString("Failed ImageLoaderSOIL::Load(). width == 0. SOIL: \"" + std::string(SOIL_last_result()) + "\"", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IImagePtr img = std::make_shared<mr::Image>();
    if(!img->Create(glm::ivec2(width, height), channels, data)) {
        mr::Log::LogString("Failed ImageLoaderSOIL::Load(). Image not created.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    return img;
}

IImagePtr ImageLoaderSOIL::Load(unsigned char* memory, size_t const& memSize, Options const& options) {
    int width, height, channels, channels_flag;

    if(options.greyscale && options.alpha) channels_flag = SOIL_LOAD_LA;
    else if(!options.greyscale && options.alpha) channels_flag = SOIL_LOAD_RGBA;
    else if(options.greyscale && !options.alpha) channels_flag = SOIL_LOAD_L;
    else if(!options.greyscale && !options.alpha) channels_flag = SOIL_LOAD_RGB;

    unsigned char* data = SOIL_load_image_from_memory(memory, memSize, &width, &height, &channels, channels_flag);

    if(data == 0) {
        mr::Log::LogString("Failed ImageLoaderSOIL::Load(). SOIL_load_image failed. SOIL: \"" + std::string(SOIL_last_result()) + "\"", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    IImagePtr img = std::make_shared<mr::Image>();
    if(!img->Create(glm::ivec2(width, height), channels, data)) {
        mr::Log::LogString("Failed ImageLoaderSOIL::Load(). Image not created.", MR_LOG_LEVEL_ERROR);
        return nullptr;
    }

    return img;
}

bool ImageSaverSOIL::Save(std::string const& path, Options const& options, const IImage* const image) {
    if(!SOIL_save_image(path.c_str(), options.fileType, image->GetWidth(), image->GetHeight(), image->GetChannelsNum(), image->GetDataRef().GetArray())) {
        mr::Log::LogString("Failed ImageSaverSOIL::Save(). SOIL_save_image failed. SOIL: \"" + std::string(SOIL_last_result()) + "\"", MR_LOG_LEVEL_ERROR);
        return false;
    }
    return true;
}

}
