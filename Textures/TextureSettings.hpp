#pragma once

#ifndef _MR_TEXTURE_SETTINGS_H_
#define _MR_TEXTURE_SETTINGS_H_

#include "TextureInterfaces.hpp"

namespace mr {

class TextureSettings : public ITextureSettings {
public:
    ITextureSettings* Copy() override;

    void SetLodBias(const float& v) override;
    void SetBorderColor(float* rgba) override;
    void SetBorderColor(const float& r, const float& g, const float& b, const float& a) override;
    void SetMinFilter(const MinFilter& v) override;
    void SetMagFilter(const MagFilter& v) override;
    void SetMinLod(const int& v) override;
    void SetMaxLod(const int& v) override;
    void SetWrapS(const Wrap& v) override;
    void SetWrapR(const Wrap& v) override;
    void SetWrapT(const Wrap& v) override;
    void SetCompareMode(const CompareMode& v) override;
    void SetCompareFunc(const CompareFunc& v) override;

    inline float GetLodBias() override {
        return _lod_bias;
    }
    inline float* GetBorderColor() override {
        return _border_color;
    }
    inline MinFilter GetMinFilter() override {
        return _min_filter;
    }
    inline MagFilter GetMagFilter() override {
        return _mag_filter;
    }
    inline int GetMinLod() override {
        return _min_lod;
    }
    inline int GetMaxLod() override {
        return _max_lod;
    }
    inline Wrap GetWrapS() override {
        return _wrap_s;
    }
    inline Wrap GetWrapR() override {
        return _wrap_r;
    }
    inline Wrap GetWrapT() override {
        return _wrap_t;
    }
    inline CompareMode GetCompareMode() override {
        return _compare_mode;
    }
    inline CompareFunc GetCompareFunc() override {
        return _compare_func;
    }

    void Create() override;
    void Destroy() override;

    TextureSettings();
    virtual ~TextureSettings();
protected:
    float _lod_bias;
    float _border_color[4];
    MinFilter _min_filter;
    MagFilter _mag_filter;
    int _min_lod, _max_lod;
    Wrap _wrap_s, _wrap_r, _wrap_t;
    CompareMode _compare_mode;
    CompareFunc _compare_func;
};

}

#endif // _MR_TEXTURE_SETTINGS_H_
