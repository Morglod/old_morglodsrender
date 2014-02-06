#pragma once

#ifndef _MR_LIGHT_H_
#define _MR_LIGHT_H_

#include "pre.hpp"

namespace MR {

class LightSource;

class ShadowMap {
public:
    enum TextureBits : unsigned char {
        tb16 = 0,
        tb24 = 1,
        tb32 = 2
    };

    inline LightSource* GetLight() { return parentLight; }
    inline int GetWidth() { return w; }
    inline int GetHeight() { return h; }

    ShadowMap(LightSource* light, const int& width, const int& height, const TextureBits& tbits = TextureBits::tb24);

protected:
    LightSource* parentLight;
    GLuint gl_texture, gl_framebuffer;
    int w, h;
};

class LightSource {
public:
    enum LightSourceType : unsigned char {
        PointLight = 0
    };

    LightSource* CreatePointLight(const glm::vec3& pos, const glm::vec4& color, const float& radius){
        LightSource* lsrc = new LightSource();
        lsrc->_pos = pos;
        lsrc->_color = color;
        lsrc->_radius = radius;
        return lsrc;
    }

private:
    LightSource(){}

protected:
    glm::vec3 _pos;
    glm::vec4 _color; //r,g,b,multiply
    float _radius;
};

}

#endif // _MR_LIGHT_H_
