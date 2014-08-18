#include "RenderTarget.hpp"
#include "Textures/TextureObjects.hpp"
#include "Utils/Log.hpp"
#include "Config.hpp"
#include "RenderManager.hpp"

#include <GL/glew.h>

namespace MR {

void RenderTarget::SetName(const std::string& n) {
    if(_name != n) {
        _name = n;
        OnNameChanged(this, n);
    }
}

void RenderTarget::CreateTargetTexture(const unsigned char & i, const MR::ITexture::InternalFormat & iFormat, const MR::ITexture::Format & Format, const MR::ITexture::Type & Type){
    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);

    MR::Texture::CreateGLTexture(&_targetTextures[i], (ITexture::InternalFormat)iFormat, _width, _height, (ITexture::Format)Format, (ITexture::Type)Type);

    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_2D, _targetTextures[i], 0);//glBindTexture(GL_TEXTURE_2D, _targetTextures[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, _targetTextures[i], 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderTarget::CreateCubeMapTargetTexture(const unsigned char & i, const MR::Texture::InternalFormat & iFormat, const MR::Texture::Format & Format, const MR::Texture::Type & Type){
    glGenTextures(1, &_targetTextures[i]);

    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    rm->SetTexture(GL_TEXTURE_CUBE_MAP, _targetTextures[i], 0);//glBindTexture(GL_TEXTURE_CUBE_MAP, _targetTextures[i]);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    std::vector<GLubyte> testData(_width * _height * 4, 128);

    for(int loop = 0; loop < 6; ++loop) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, (int)iFormat,
            _width, _height, 0, (unsigned int)Format, (unsigned int)Type, &testData[0]);
    }

    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
    for(int face = 0; face < 6; face++) {
        //draw();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_CUBE_MAP_POSITIVE_X + face, _targetTextures[i], 0);
    }
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
    MR::RenderManager* rm = MR::RenderManager::GetInstance();

    MR::RenderManager::TextureSlot unbindedTexture;
    rm->UnBindTexture(0, &unbindedTexture);

    MR::Texture::CreateGLTexture(&_targetTextures[targetTexturesNum], (Texture::InternalFormat)GL_DEPTH_COMPONENT32, _width, _height, (Texture::Format)GL_DEPTH_COMPONENT, (Texture::Type)GL_FLOAT);
    rm->SetTexture(GL_TEXTURE_2D, _targetTextures[targetTexturesNum], 0);//glBindTexture(GL_TEXTURE_2D, _targetTextures[targetTexturesNum]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    rm->SetTexture(&unbindedTexture);//glBindTexture(GL_TEXTURE_2D, 0);
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
