#include "RenderTarget.hpp"
#include "Texture.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace MR {

void RenderTarget::CreateTargetTexture(const unsigned char & i, const GLenum & iFormat, const GLenum & Format, const GLenum & Type){
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

    MR::Texture::CreateOpenGLTexture(&_targetTextures[i], (Texture::InternalFormat)iFormat, _width, _height, (Texture::Format)Format, (Texture::Type)Type);
    glBindTexture(GL_TEXTURE_2D, _targetTextures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, _targetTextures[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::RenderTarget(const std::string& n, const unsigned char & targetTexturesNum, const unsigned short & Width, const unsigned short & Height) : _name(n), _width(Width), _height(Height), _frameBuffer(0), _targetTextures(nullptr), _drawBuffers(nullptr), _targetTexNum(targetTexturesNum) {
    glGenFramebuffers(1, &_frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

    /*glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);*/

    _targetTextures = new GLuint[targetTexturesNum+1];

    //------CREATE DEPTH TEXTURE--
    MR::Texture::CreateOpenGLTexture(&_targetTextures[targetTexturesNum], (Texture::InternalFormat)GL_DEPTH_COMPONENT32, _width, _height, (Texture::Format)GL_DEPTH_COMPONENT, (Texture::Type)GL_FLOAT);
    glBindTexture(GL_TEXTURE_2D, _targetTextures[targetTexturesNum]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, _targetTextures[targetTexturesNum], 0);
    //----------------------------

    _drawBuffers = new GLenum[targetTexturesNum];
    for(unsigned char i = 0; i < targetTexturesNum; ++i){
        _drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
    }

    glDrawBuffers(targetTexturesNum, _drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        MR::Log::LogString("Failed frame buffer creation in RenderTarget", MR_LOG_LEVEL_ERROR);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget(){
    delete _drawBuffers;
    //glDeleteRenderbuffers(1, &depthrenderbuffer);
    glDeleteTextures(_targetTexNum+1, _targetTextures);
    glDeleteFramebuffers(1, &_frameBuffer);
}

}
