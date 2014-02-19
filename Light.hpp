#pragma once

#ifndef _MR_LIGHT_H_
#define _MR_LIGHT_H_

#include "Config.hpp"

#ifndef glm_glm
#   include <glm/glm.hpp>
#endif

namespace MR {

class LightSource;

class ShadowMap {
public:
    enum TextureBits : unsigned char {
        tb16 = 0,
        tb24 = 1,
        tb32 = 2
    };

    inline LightSource* GetLight() { return _parentLight; }
    inline int GetWidth() { return _w; }
    inline int GetHeight() { return _h; }

    ShadowMap(LightSource* light, const int& width, const int& height, const TextureBits& tbits = TextureBits::tb24);

protected:
    LightSource* _parentLight;
    unsigned int _gl_texture, _gl_framebuffer;
    int _w, _h;
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
