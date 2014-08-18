#pragma once

#ifndef _MR_TEXTURE_OBJECTS_H_
#define _MR_TEXTURE_OBJECTS_H_

#include "TextureInterfaces.hpp"
#include "TextureLoader.hpp"

namespace MR {

class Texture : public ITexture {
public:
    inline ITextureSettings* GetSettings() override {
        return _settings;    //May be nullptr
    }
    inline unsigned short GetWidth() override {
        return gl_width;
    }
    inline unsigned short GetHeight() override {
        return gl_height;
    }
    inline unsigned short GetDepth() override {
        return gl_depth;
    }
    inline InternalFormat GetInternalFormat() override {
        return (InternalFormat)gl_internal_format;
    }
    inline unsigned int GetImageSize() override {
        return gl_mem_image_size;
    }
    inline unsigned char GetRedBitsNum() override {
        return gl_red_bits_num;
    }
    inline unsigned char GetGreenBitsNum() override {
        return gl_green_bits_num;
    }
    inline unsigned char GetBlueBitsNum() override {
        return gl_blue_bits_num;
    }
    inline unsigned char GetDepthBitsNum() override {
        return gl_depth_bits_num;
    }
    inline unsigned char GetAlphaBitsNum() override {
        return gl_alpha_bits_num;
    }
    inline bool IsCompressed() override {
        return gl_compressed;
    }
    inline unsigned int GetGPUHandle() override {
        return this->gl_texture;
    }
    inline Target GetTarget() override {
        return _target;
    }
    inline ITexture::CompressionMode GetCompressionMode() {
        return _compression_mode;
    }
    inline void SetSettings(ITextureSettings* ts) {
        if(_settings != ts) {
            _settings = ts;
            OnSettingsChanged(this, ts);
        }
    }

    void ResetInfo(); //Get fresh info about texture

    void GetData(const int& mipMapLevel, const Format& format, const Type& type, void* dstBuffer);
    void SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data);
    void UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data);

    Texture(const unsigned int& gpu_handle, const ITexture::Target& target);
    virtual ~Texture();

    static void CreateGLTexture(unsigned int* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type);
    static void CreateGLTexture(const unsigned char *const data, const int& width, const int& height, const TextureLoader::TextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const TextureLoader::TextureLoadFlags& flags = TextureLoader::LFL_MipMaps);
    static void CreateGLTexture(const unsigned char *const data, const char face_order[6], const int& width, const int& height, const TextureLoader::TextureLoadFormat& format, unsigned int * TexDst = new unsigned int(0), const TextureLoader::TextureLoadFlags& flags = TextureLoader::LFL_MipMaps);

protected:

    unsigned short gl_width = 0, gl_height = 0, gl_depth = 0;
    unsigned char gl_red_bits_num = 0, gl_green_bits_num = 0, gl_blue_bits_num = 0, gl_depth_bits_num = 0, gl_alpha_bits_num = 0;
    unsigned int gl_mem_image_size = 0, gl_internal_format = 0;
    bool gl_compressed = false;
    unsigned int gl_texture; //OpenGL texture
    ITextureSettings* _settings = nullptr;
    ITexture::CompressionMode _compression_mode;
    Target _target;
};

}

#endif // _MR_TEXTURE_OBJECTS_H_
