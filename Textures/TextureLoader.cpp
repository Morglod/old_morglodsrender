#include "TextureLoader.hpp"
#include "../Utils/Threads.hpp"
#include "../Utils/Exception.hpp"
#include "../Utils/Log.hpp"

#ifndef HEADER_SIMPLE_OPENGL_IMAGE_LIBRARY
#   include <SOIL.h>
#endif

namespace MR {


unsigned int TextureLoader::LoadGLTexture(const std::string& file, const unsigned int & t, const TextureLoadFormat& format, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_texture(file.c_str(),(int)format, t,(unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLTexture(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLTexture(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLCubemap(const std::string& x_pos_file, const std::string& x_neg_file, const std::string& y_pos_file, const std::string& y_neg_file, const std::string& z_pos_file, const std::string& z_neg_file,
                               const unsigned int & t, const TextureLoadFormat& format, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_cubemap(x_pos_file.c_str(), x_neg_file.c_str(), y_pos_file.c_str(), y_neg_file.c_str(), z_pos_file.c_str(), z_neg_file.c_str(), (int)format, t,(unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLCubemap(const std::string& file, const char faceOrder[6], const unsigned int & t, const TextureLoadFormat& format, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_single_cubemap(file.c_str(), faceOrder, (int)format, t, (unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLHDR(const std::string& file, const int& rescale_to_max, const unsigned int & t, const TextureHDRFake& hdrFake, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_HDR_texture(file.c_str(), (int)hdrFake, rescale_to_max, t, (unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLHDR(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLHDR(\""+file+"\"). " + lastResult);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLTexture(const unsigned char *const buffer, const unsigned int& buffer_length, const unsigned int & t, const TextureLoadFormat& format, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_texture_from_memory(buffer, buffer_length, (int)format, t,(unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLTexture. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLCubemap(const unsigned char *const x_pos_buffer, const unsigned int& x_pos_buffer_length,
                               const unsigned char *const x_neg_buffer, const unsigned int& x_neg_buffer_length,
                               const unsigned char *const y_pos_buffer, const unsigned int& y_pos_buffer_length,
                               const unsigned char *const y_neg_buffer, const unsigned int& y_neg_buffer_length,
                               const unsigned char *const z_pos_buffer, const unsigned int& z_pos_buffer_length,
                               const unsigned char *const z_neg_buffer, const unsigned int& z_neg_buffer_length,
                               const unsigned int & t,
                               const TextureLoadFormat& format, const TextureLoadFlags& flags){

    unsigned int i = SOIL_load_OGL_cubemap_from_memory(x_pos_buffer, x_pos_buffer_length,
                                                       x_neg_buffer, x_neg_buffer_length,
                                                       y_pos_buffer, y_pos_buffer_length,
                                                       y_neg_buffer, y_neg_buffer_length,
                                                       z_pos_buffer, z_pos_buffer_length,
                                                       z_neg_buffer, z_neg_buffer_length, (int)format, t,(unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

unsigned int TextureLoader::LoadGLCubemap(const unsigned char *const buffer, const unsigned int& buffer_length, const char faceOrder[6], const unsigned int & t, const TextureLoadFormat& format, const TextureLoadFlags& flags){
    unsigned int i = SOIL_load_OGL_single_cubemap_from_memory(buffer, buffer_length, faceOrder, (int)format, t, (unsigned int)flags);
    if(i == 0){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadGLCubemap. " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadGLCubemap. " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return i;
}

bool TextureLoader::SaveScreenshot(const std::string& file, const ImageSaveType& image_type, const int& x, const int& y, const int& width, const int& height){
    bool state = SOIL_save_screenshot(file.c_str(), (int)image_type, x, y, width, height);
    if(state == false){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed SaveScreenshot(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed SaveScreenshot(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return state;
}

MR::Mutex __LOAD_IMAGE_MUTEX;

unsigned char* TextureLoader::LoadImage(const std::string& file, int* width, int* height, TextureLoadFormat* format, const TextureLoadFormat& force_format){
    __LOAD_IMAGE_MUTEX.Lock();
    unsigned char* ptr = SOIL_load_image(file.c_str(), width, height, (int*)format, (int)force_format);
    __LOAD_IMAGE_MUTEX.UnLock();
    if(ptr == NULL){
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()){
            throw MR::Exception("Failed LoadImage(\""+file+"\"). " + lastResult);
        } else {
            MR::Log::LogString("Failed LoadImage(\""+file+"\"). " + lastResult, MR_LOG_LEVEL_ERROR);
        }
    }
    return ptr;
}

TextureLoader::TextureLoader() {
}

TextureLoader::~TextureLoader() {
}

}
