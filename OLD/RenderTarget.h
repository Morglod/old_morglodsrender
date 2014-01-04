#pragma once

#ifndef _MR_RENDER_TARGET_H_
#define _MR_RENDER_TARGET_H_

//#include "Texture.h"

namespace MR{
    /*class RenderTarget;

    typedef std::shared_ptr<MR::RenderTarget> RenderTargetPtr;

    class RenderTarget : public virtual MR::Texture{
    public:
        int Width = 640;
        int Height = 480;
        bool SizeChanged = false;

        GLuint framebuffer = 0;

        inline RenderTargetPtr GetHandle(){return std::shared_ptr<RenderTarget>(this);}

        void Use();

        RenderTarget(ResourceManagerPtr manager, std::string source, std::string name = "ResourceRenderTarget");
        ~RenderTarget();
    };*/
}

#endif // _MR_RENDER_TARGET_H_
