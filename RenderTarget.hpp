#pragma once

#ifndef _MR_RENDER_TARGET_H_
#define _MR_RENDER_TARGET_H_

#include "pre.hpp"
#include "Events.hpp"
#include "RenderContext.hpp"

namespace MR {

class RenderTarget {
private:
    std::string name;
    unsigned short width, height;

protected:
    GLuint frameBuffer;
    GLuint* targetTextures;
    GLenum* drawBuffers;//{GL_COLOR_ATTACHMENT0};
    unsigned char targetTexNum;
    GLuint depthrenderbuffer;

public:
    MR::Event<const std::string&> OnNameChanged;

    inline std::string GetName(){ return name; }
    inline void SetName(const std::string& n){
        if(name != n) {
            name = n;
            OnNameChanged(this, n);
        }
    }

    inline void Bind(RenderContext& rc){
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        rc.SetViewport(0, 0, width, height);
    }

    inline void Unbind(){ glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    inline unsigned short GetWidth(){ return width; }
    inline unsigned short GetHeight(){ return height; }
    inline GLuint GetTargetTexture(const unsigned char & i){ return targetTextures[i]; }

    /** i - index of texture in (targetTextures)
        iFormat - internal format
    */
    void CreateTargetTexture(const unsigned char & i,const GLenum & iFormat, const GLenum & Format, const GLenum & Type);

    /** TargetTexturesNum - 15 Max
    */
    RenderTarget(const std::string& Name, const unsigned char & TargetTexturesNum, const unsigned short & Width, const unsigned short & Height);
    ~RenderTarget();
};

}

#endif
