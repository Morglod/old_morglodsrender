#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

#include "pre.hpp"

#include "GeometryBuffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Events.hpp"

namespace MR {

class RenderContext {
protected:
    GeometryBuffer* _gb = nullptr;
    Shader* _sh = nullptr;
    Texture* _tx[GL_TEXTURE31-GL_TEXTURE0+1];

public:

    inline void UseGeometryBuffer(GeometryBuffer* gb) {
        if(gb != _gb) {
            _gb = gb;
            _gb->Bind();
        }
    }

    inline void UseShader(Shader* sh) {
        if(_sh != sh) {
            _sh = sh;
            _sh->Use();
        }
    }

    inline void UseTexture(Texture* tx, GLenum texStage) {
        if(_tx[texStage-GL_TEXTURE0] != tx) {
            _tx[texStage-GL_TEXTURE0] = tx;
            tx->Bind(texStage);
        }
    }

    inline void DrawGeometryBuffer(GeometryBuffer* gb) {
        UseGeometryBuffer(gb);
        _gb->Draw();
    }

    virtual bool Init();

    //Calculates frames per second
    //timeType is data type of time in calculations
    template<typename timeType>
    static unsigned short inline FPS(){
        static timeType lastTime = 0.0;
        static timeType time = 0.0;
        static unsigned short frames = 0;
        static unsigned short lastFps = 0;

        timeType currentTime = (timeType)glfwGetTime();
        time += currentTime-lastTime;
        lastTime = currentTime;
        ++frames;
        if(time > 1){
            lastFps = frames;
            time -= 1.0;
            frames = 0;
        }

        return lastFps;
    }

    RenderContext();
    ~RenderContext();
};

}

#endif
