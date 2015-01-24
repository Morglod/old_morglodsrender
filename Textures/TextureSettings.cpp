#include "TextureSettings.hpp"
#include "../Config.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

void mr::TextureSettings::Create() {
    if(_handle != 0) return;
    if(mr::gl::IsOpenGL45()) glCreateSamplers(1, &_handle);
    else glGenSamplers(1, &_handle);
    glSamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, _lod_bias);
    glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, (int)_mag_filter);
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, (int)_min_filter);
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_LOD, _min_lod);
    glSamplerParameteri(_handle, GL_TEXTURE_MAX_LOD, _max_lod);
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, (int)_wrap_s);
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, (int)_wrap_r);
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, (int)_wrap_t);
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_MODE, (int)_compare_mode);
    glSamplerParameterf(_handle, GL_TEXTURE_COMPARE_FUNC, (int)_compare_func);
}

void mr::TextureSettings::SetLodBias(const float& v) {
    Create();
    glSamplerParameterf(_handle, GL_TEXTURE_LOD_BIAS, v);
    _lod_bias = v;
    OnLodBiasChanged(this, v);
}

void mr::TextureSettings::SetBorderColor(float* rgba) {
    Create();
    glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, rgba);
    _border_color[0] = rgba[0];
    _border_color[1] = rgba[1];
    _border_color[2] = rgba[2];
    _border_color[3] = rgba[3];
    OnBorderColorChanged(this, _border_color);
}

void mr::TextureSettings::SetBorderColor(const float& r, const float& g, const float& b, const float& a) {
    Create();
    _border_color[0] = r;
    _border_color[1] = g;
    _border_color[2] = b;
    _border_color[3] = a;
    glSamplerParameterfv(_handle, GL_TEXTURE_BORDER_COLOR, _border_color);
    OnBorderColorChanged(this, _border_color);
}

void mr::TextureSettings::SetMagFilter(const mr::TextureSettings::MagFilter& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_MAG_FILTER, (int)v);
    _mag_filter = v;
    OnMagFilterChanged(this, v);
}

void mr::TextureSettings::SetMinFilter(const mr::TextureSettings::MinFilter& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_FILTER, (int)v);
    _min_filter = v;
    OnMinFilterChanged(this, v);
}

void mr::TextureSettings::SetMinLod(const int& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_MIN_LOD, v);
    _min_lod = v;
    OnMinLodChanged(this, v);
}

void mr::TextureSettings::SetMaxLod(const int& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_MAX_LOD, v);
    _max_lod = v;
    OnMaxLodChanged(this, v);
}

void mr::TextureSettings::SetWrapS(const Wrap& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_S, (int)v);
    _wrap_s = v;
    OnWrapSChanged(this, v);
}

void mr::TextureSettings::SetWrapR(const Wrap& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_R, (int)v);
    _wrap_r = v;
    OnWrapRChanged(this, v);
}

void mr::TextureSettings::SetWrapT(const Wrap& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_WRAP_T, (int)v);
    _wrap_t = v;
    OnWrapTChanged(this, v);
}

void mr::TextureSettings::SetCompareMode(const CompareMode& v) {
    Create();
    glSamplerParameteri(_handle, GL_TEXTURE_COMPARE_MODE, (int)v);
    _compare_mode = v;
    OnCompareModeChanged(this, v);
}

void mr::TextureSettings::SetCompareFunc(const CompareFunc& v) {
    Create();
    glSamplerParameterf(_handle, GL_TEXTURE_COMPARE_FUNC, (int)v);
    _compare_func = v;
    OnCompareFuncChanged(this, v);
}

mr::ITextureSettings* mr::TextureSettings::Copy(){
    TextureSettings* ts = new TextureSettings();
    ts->SetLodBias(_lod_bias);
    ts->SetBorderColor(&_border_color[0]);
    ts->SetMagFilter(_mag_filter);
    ts->SetMinFilter(_min_filter);
    ts->SetMinLod(_min_lod);
    ts->SetMaxLod(_max_lod);
    ts->SetWrapS(_wrap_s);
    ts->SetWrapT(_wrap_t);
    ts->SetWrapR(_wrap_r);
    ts->SetCompareMode(_compare_mode);
    ts->SetCompareFunc(_compare_func);
    return ts;
}

void mr::TextureSettings::Destroy() {
    if(_handle != 0) {
        glDeleteBuffers(1, &_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
        OnDestroy(dynamic_cast<mr::IObjectHandle*>(this));
    }
}

mr::TextureSettings::TextureSettings() :
    _lod_bias(MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS),
    _border_color{MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR},
              _min_filter((MinFilter)MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER),
              _mag_filter((MagFilter)MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER),
              _min_lod(MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD),
              _max_lod(MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD),
              _wrap_s((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S),
              _wrap_r((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R),
              _wrap_t((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T),
              _compare_mode((CompareMode)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE),
_compare_func((CompareFunc)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC) {
}

mr::TextureSettings::~TextureSettings() {
}
