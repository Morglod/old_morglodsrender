#pragma once

#ifndef _MR_CAMERA_H_
#define _MR_CAMERA_H_

#include "Node.h"
//#include "RenderTarget.h"

namespace MR{
    class RenderTarget;

    //typedef std::shared_ptr<MR::RenderTarget> RenderTargetPtr;

    class Camera : public virtual MR::Node{
    public:
        //MR::RenderTargetPtr renderTarget = nullptr;

        float zNear = 0.1f;
        float zFar = 1000.0f;
        float FovY = 45.0f;
        float AspectRatio = 640 / 480;

        float cameraX = 0;
        float cameraY = 0;
        float cameraZ = 0;

        float upX = 0;
        float upY = 1;
        float upZ = 0;

        void Use();

        inline std::type_index GetType(){
            return typeid(MR::Camera);
        }
    };
}

#endif // _MR_CAMERA_H_
