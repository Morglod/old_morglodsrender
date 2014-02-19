#pragma once

#ifndef _MR_RENDER_TARGET_H_
#define _MR_RENDER_TARGET_H_

#include "pre.hpp"
#include "Events.hpp"
#include "RenderContext.hpp"

namespace MR {

class RenderTarget {
public:
    MR::Event<const std::string&> OnNameChanged;

    inline std::string GetName(){ return _name; }
    inline void SetName(const std::string& n){
        if(_name != n) {
            _name = n;
            OnNameChanged(this, n);
        }
    }

    inline void Bind(RenderContext& rc){
        glBindFramebuffer(GL_FRAMEBUFFER, _frameBuffer);
        rc.SetViewport(0, 0, _width, _height);
    }

    inline void Unbind(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    inline unsigned short GetWidth(){ return _width; }
    inline unsigned short GetHeight(){ return _height; }
    inline unsigned int GetTargetTexture(const unsigned char & i){ return _targetTextures[i]; }

    /** i - index of texture in (targetTextures)
        iFormat - internal format
    */
    void CreateTargetTexture(const unsigned char & i,const unsigned int & iFormat, const unsigned int & Format, const unsigned int & Type);

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

#endif
