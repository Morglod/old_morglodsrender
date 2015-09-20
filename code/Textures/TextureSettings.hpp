#pragma once

#ifndef _MR_TEXTURE_SETTINGS_H_
#define _MR_TEXTURE_SETTINGS_H_

#include "../CoreObjects.hpp"

#include "../pre_glm.hpp"

namespace mr {

class TextureSettings : public IGPUObjectHandle {
    friend class TextureManager;
public:
    enum MinFilter : int {
        MinFilter_NEAREST_MIPMAP_NEAREST = 0x2700,
        MinFilter_LINEAR_MIPMAP_NEAREST = 0x2701,
        MinFilter_NEAREST_MIPMAP_LINEAR = 0x2702,
        MinFilter_LINEAR_MIPMAP_LINEAR = 0x2703,
        MinFilter_NEAREST = 0x2600,
        MinFilter_LINEAR = 0x2601
    };

    enum MagFilter : int {
        MagFilter_NEAREST = 0x2600,
        MagFilter_LINEAR = 0x2601
    };

    enum Wrap : int {
        Wrap_CLAMP = 0x812F,
        Wrap_REPEAT = 0x2901,
        Wrap_MIRRORED_REPEAT = 0x8370,
        Wrap_DECAL = 0x2101
    };

    enum CompareMode : int {
        CompareMode_REF_TO_TEXTURE = 0x884E,
        CompareMode_NONE = 0
    };

    enum CompareFunc : int {
        CompareFunc_LEQUAL = 0x0203,
        CompareFunc_GEQUAL = 0x0206,
        CompareFunc_LESS = 0x0201,
        CompareFunc_GREATHER = 0x0204,
        CompareFunc_EQUAL = 0x0202,
        CompareFunc_NOTEQUAL = 0x0205,
        CompareFunc_ALWAYS = 0x0207,
        CompareFunc_NEVER = 0x0200
    };

    struct Desc {
        float lod_bias;
        glm::vec4 border_color;
        MinFilter min_filter;
        MagFilter mag_filter;
        int lod_min, lod_max;
        Wrap wrap_s, wrap_r, wrap_t;
        CompareMode compare_mode;
        CompareFunc compare_func;

        Desc();
    };

    virtual bool Set(Desc const& desc);
    virtual bool Get(Desc* desc);

    virtual void Destroy();

    TextureSettings();
    virtual ~TextureSettings();
protected:
    virtual bool Create();
    Desc _desc;
};

}

#endif // _MR_TEXTURE_SETTINGS_H_
