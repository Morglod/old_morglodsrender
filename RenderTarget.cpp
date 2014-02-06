#include "RenderTarget.hpp"
#include "Texture.hpp"

namespace MR {

void RenderTarget::CreateTargetTexture(const unsigned char & i, const GLenum & iFormat, const GLenum & Format, const GLenum & Type){
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    MR::Texture::CreateOpenGLTexture(&targetTextures[i], iFormat, width, height, Format, Type);
    glBindTexture(GL_TEXTURE_2D, targetTextures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, targetTextures[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::RenderTarget(const std::string& n, const unsigned char & targetTexturesNum, const unsigned short & Width, const unsigned short & Height) : name(n), width(Width), height(Height), frameBuffer(0), targetTextures(nullptr), drawBuffers(nullptr), targetTexNum(targetTexturesNum) {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    /*glGenRenderbuffers(1, &depthrenderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthrenderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthrenderbuffer);*/

    targetTextures = new GLuint[targetTexturesNum+1];

    //------CREATE DEPTH TEXTURE--
    MR::Texture::CreateOpenGLTexture(&targetTextures[targetTexturesNum], GL_DEPTH_COMPONENT32, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
    glBindTexture(GL_TEXTURE_2D, targetTextures[targetTexturesNum]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, targetTextures[targetTexturesNum], 0);
    //----------------------------

    drawBuffers = new GLenum[targetTexturesNum];
    for(unsigned char i = 0; i < targetTexturesNum; ++i){
        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
    }

    glDrawBuffers(targetTexturesNum, drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        MR::Log::LogString("Failed frame buffer creation in RenderTarget", MR_LOG_LEVEL_ERROR);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

RenderTarget::~RenderTarget(){
    delete drawBuffers;
    //glDeleteRenderbuffers(1, &depthrenderbuffer);
    glDeleteTextures(targetTexNum+1, targetTextures);
    glDeleteFramebuffers(1, &frameBuffer);
}

}
