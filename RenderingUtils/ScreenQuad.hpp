#pragma once

#ifndef _MR_SCREEN_QUAD_H_
#define _MR_SCREEN_QUAD_H_

namespace MR {

class IGeometry;
class ITexture;
class IShaderProgram;
class IShaderUniform;

class ScreenQuad {
public:
    void Draw();
    virtual void SetTexture(ITexture* tex) { _texture = tex; }

    ScreenQuad();
    virtual ~ScreenQuad();
protected:
    IGeometry* _quad;
    ITexture* _texture;
    IShaderProgram* _prog;
    IShaderUniform* _ambientTex;
    int _ambientTexUnit;
};

}

#endif // _MR_SCREEN_QUAD_H_
