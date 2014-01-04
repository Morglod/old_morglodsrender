#pragma once

#ifndef _MR_RENDER_MANAGER_H_
#define _MR_RENDER_MANAGER_H_

//GLEW
#ifndef __glew_h__
#   define GLEW_STATIC
#   include <GL\glew.h>
#endif

#include "GeometryBuffer.hpp"
#include "Shader.hpp"

namespace MR {
class RenderManager {
protected:
    GeometryBuffer* _gb = nullptr;
    Shader* _sh = nullptr;
    Texture* _tx = nullptr;
    GLenum _tx_stage = GL_TEXTURE0;
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
        if( (_tx != tx) || (_tx_stage != texStage) ) {
            _tx = tx;
            _tx_stage = texStage;
            _tx->Bind(_tx_stage);
        }
    }

    inline void DrawGeometryBuffer(GeometryBuffer* gb) {
        UseGeometryBuffer(gb);
        _gb->Draw();
    }

    static RenderManager* Instance(){
        static RenderManager* i = new RenderManager();
        return i;
    }
};
}

#endif
