#include "TextureSettings.hpp"
#include "../Config.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace mr {

TextureSettings::Desc::Desc() :
              lod_bias(MR_DEFAULT_TEXTURE_SETTINGS_LOD_BIAS),
              border_color{MR_DEFAULT_TEXTURE_SETTINGS_BORDER_COLOR},
              min_filter((MinFilter)MR_DEFAULT_TEXTURE_SETTINGS_MIN_FILTER),
              mag_filter((MagFilter)MR_DEFAULT_TEXTURE_SETTINGS_MAG_FILTER),
              lod_min(MR_DEFAULT_TEXTURE_SETTINGS_MIN_LOD),
              lod_max(MR_DEFAULT_TEXTURE_SETTINGS_MAX_LOD),
              wrap_s((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_S),
              wrap_r((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_R),
              wrap_t((Wrap)MR_DEFAULT_TEXTURE_SETTINGS_WRAP_T),
              compare_mode((CompareMode)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_MODE),
              compare_func((CompareFunc)MR_DEFAULT_TEXTURE_SETTINGS_COMPARE_FUNC)
{  }

bool TextureSettings::Create() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) return true;
    if(mr::gl::IsOpenGL45()) glCreateSamplers(1, &handle);
    else glGenSamplers(1, &handle);
    SetGPUHandle(handle);
    return Set(Desc());
}

bool TextureSettings::Set(Desc const& desc) {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) return false;
    glSamplerParameterf(handle, GL_TEXTURE_LOD_BIAS, desc.lod_bias);
    glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, (int)desc.mag_filter);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, (int)desc.min_filter);
    glSamplerParameteri(handle, GL_TEXTURE_MIN_LOD, desc.lod_min);
    glSamplerParameteri(handle, GL_TEXTURE_MAX_LOD, desc.lod_max);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_S, (int)desc.wrap_s);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_R, (int)desc.wrap_r);
    glSamplerParameteri(handle, GL_TEXTURE_WRAP_T, (int)desc.wrap_t);
    glSamplerParameteri(handle, GL_TEXTURE_COMPARE_MODE, (int)desc.compare_mode);
    glSamplerParameterf(handle, GL_TEXTURE_COMPARE_FUNC, (int)desc.compare_func);
    glSamplerParameterfv(GetGPUHandle(), GL_TEXTURE_BORDER_COLOR, &desc.border_color[0]);
    _desc = desc;
    return true;
}

bool TextureSettings::Get(Desc* desc) {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) return false;
    *desc = _desc;
    return true;
}

void TextureSettings::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteBuffers(1, &handle);
        SetGPUHandle(0);
    }
}

TextureSettings::TextureSettings() {
}

TextureSettings::~TextureSettings() {
}

}
