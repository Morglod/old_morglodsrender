#include "Light.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

MR::ShadowMap::ShadowMap(LightSource* light, const int& width, const int& height, const TextureBits& tbits) :
    _parentLight(light), _gl_texture(0), _gl_framebuffer(0), _w(width), _h(height) {
    glGenTextures(1, &_gl_texture);
    glBindTexture(GL_TEXTURE_2D, _gl_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    if(tbits == TextureBits::tb16) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    else if(tbits == TextureBits::tb24) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    else if(tbits == TextureBits::tb32) glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &_gl_framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _gl_framebuffer);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _gl_texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
