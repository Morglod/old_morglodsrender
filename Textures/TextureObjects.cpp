#include "TextureObjects.hpp"
#include "../Utils/Exception.hpp"
#include "../Utils/Log.hpp"
#include "../RenderManager.hpp"

#include <SOIL.h>
#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void MR::Texture::GetData(const int& mipMapLevel, const MR::Texture::Format& format, const MR::Texture::Type& type, void* dstBuffer) {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, gl_texture, 0);//glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexImage(GL_TEXTURE_2D, mipMapLevel, (int)format, (int)type, dstBuffer);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
}

void MR::Texture::SetData(const int& mipMapLevel, const InternalFormat& internalFormat, const int& width, const int& height, const Format& format, const Type& type, void* data) {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, gl_texture, 0);//glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexImage2D(GL_TEXTURE_2D,
                 mipMapLevel,
                 (int)internalFormat,
                 width,
                 height,
                 0,
                 (int)format,
                 (int)type,
                 data);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
    OnDataChanged(this, mipMapLevel, internalFormat, width, height, format, type, data);
}

void MR::Texture::UpdateData(const int& mipMapLevel, const int& xOffset, const int& yOffset, const int& width, const int& height, const Format& format, const Type& type, void* data) {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, gl_texture, 0);//glBindTexture(GL_TEXTURE_2D, gl_texture);
    glTexSubImage2D(GL_TEXTURE_2D,
                    mipMapLevel,
                    xOffset,
                    yOffset,
                    width,
                    height,
                    (int)format,
                    (int)type,
                    data);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
    OnDataUpdated(this, mipMapLevel, xOffset, yOffset, width, height, format, type, data);
}

void MR::Texture::ResetInfo() {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, gl_texture, 0);//glBindTexture(GL_TEXTURE_2D, gl_texture);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (int*)(&gl_width));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (int*)(&gl_height));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, (int*)(&gl_depth));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, (int*)(&gl_internal_format));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, (int*)(&gl_red_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, (int*)(&gl_green_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, (int*)(&gl_blue_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, (int*)(&gl_depth_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, (int*)(&gl_alpha_bits_num));
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, (int*)(&gl_compressed));
    gl_mem_image_size = ((gl_red_bits_num+gl_green_bits_num+gl_blue_bits_num+gl_depth_bits_num+gl_alpha_bits_num)/8)*gl_width*gl_height;
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
    OnInfoReset(this);
}

void MR::Texture::CreateGLTexture(GLuint* TexDst, const InternalFormat& internalFormat, const int& Width, const int& Height, const Format& format, const Type& type) {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    glGenTextures(1, TexDst);
    rm->SetTexture(GL_TEXTURE_2D, *TexDst, 0);//glBindTexture(GL_TEXTURE_2D, *TexDst);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, (int)internalFormat, Width, Height, 0, (int)format, (int)type, 0);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
}

void MR::Texture::CreateGLTexture(const unsigned char *const data, const int& width, const int& height, const MR::TextureLoader::TextureLoadFormat& format, unsigned int * TexDst, const MR::TextureLoader::TextureLoadFlags& flags) {
    unsigned int t = 0;
    if((*TexDst) != 0) t = *TexDst;
    *TexDst = SOIL_create_OGL_texture(data, width, height, (int)format, t, (unsigned int)flags);
    if((*TexDst) == 0) {
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()) {
            throw MR::Exception("Failed CreateGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed CreateGLTexture. " + lastResult);
        }
    }
}

void MR::Texture::CreateGLTexture(const unsigned char *const data, const char face_order[6], const int& width, const int& height, const MR::TextureLoader::TextureLoadFormat& format, unsigned int * TexDst, const MR::TextureLoader::TextureLoadFlags& flags) {
    unsigned int t = 0;
    if((*TexDst) != 0) t = *TexDst;
    *TexDst = SOIL_create_OGL_single_cubemap(data, width, height, (int)format, face_order, t, (unsigned int)flags);
    if((*TexDst) == 0) {
        std::string lastResult = std::string(SOIL_last_result());
        if(ThrowExceptions()) {
            throw MR::Exception("Failed CreateGLTexture. " + lastResult);
        } else {
            MR::Log::LogString("Failed CreateGLTexture. " + lastResult);
        }
    }
}

void MR::Texture::Destroy() {
    if(gl_texture != 0){
        glDeleteTextures(1, &gl_texture);
        gl_texture = 0;
    }
}

MR::Texture::Texture(const unsigned int& handle, const MR::ITexture::Target& target) : gl_texture(handle), _settings(nullptr), _target(target) {
    if(handle) ResetInfo();
}

MR::Texture::~Texture() {
}
/*
void MR::TextureBinded::UnBind() {
    MR::RenderManager* rm = MR::RenderManager::GetInstance();
    rm->UnBindTexture(i, nullptr);
}

MR::TextureBinded::TextureBinded(ITexture* t, int const& i_) : tex(t), i(i_) {
}

void MR::TextureBindedNV::UnBind() {
}

MR::TextureBindedNV::TextureBindedNV(ITexture* t, uint64_t const& i_) : tex(t), nv(i_) {
}
*/
