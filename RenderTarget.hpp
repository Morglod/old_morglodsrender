#pragma once

#ifndef _MR_RENDER_TARGET_H_
#define _MR_RENDER_TARGET_H_

#include "Events.hpp"
#include "RenderContext.hpp"

#include <string>

namespace MR {

class RenderTarget {
public:
    MR::Event<const std::string&> OnNameChanged;

    inline std::string GetName();
    void SetName(const std::string& n);

    void Bind(RenderContext& rc);
    void Bind(RenderContext* rc);

    void Unbind();
    inline unsigned short GetWidth();
    inline unsigned short GetHeight();
    inline unsigned int GetTargetTexture(const unsigned char & i);

    /** i - index of texture in (targetTextures)
        iFormat - internal format
    */
    void CreateTargetTexture        (const unsigned char & i, const MR::Texture::InternalFormat & iFormat, const MR::Texture::Format & Format, const MR::Texture::Type & Type);
    void CreateCubeMapTargetTexture (const unsigned char & i, const MR::Texture::InternalFormat & iFormat, const MR::Texture::Format & Format, const MR::Texture::Type & Type);

    /** TargetTexturesNum - 15 Max
    */
    RenderTarget(const std::string& Name, const unsigned char & TargetTexturesNum, const unsigned short & Width, const unsigned short & Height);
    ~RenderTarget();

private:
    std::string _name;
    unsigned short _width, _height;
protected:
    unsigned int _frameBuffer;
    unsigned int * _targetTextures;
    unsigned int * _drawBuffers;//{GL_COLOR_ATTACHMENT0};
    unsigned char _targetTexNum;
    unsigned int _depthrenderbuffer;
};

}

std::string MR::RenderTarget::GetName() {
    return _name;
}

unsigned short MR::RenderTarget::GetWidth() {
    return _width;
}

unsigned short MR::RenderTarget::GetHeight() {
    return _height;
}

unsigned int MR::RenderTarget::GetTargetTexture(const unsigned char & i) {
    return _targetTextures[i];
}

#endif
