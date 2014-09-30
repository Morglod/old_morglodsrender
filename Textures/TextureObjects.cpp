#include "TextureObjects.hpp"
#include "../Utils/Exception.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"

#include <SOIL.h>
#ifndef __glew_h__
#   include <GL\glew.h>
#endif

MR::TStaticArray<MR::ITexture*> _MR_TEXTURE_BIND_TARGETS_;
MR::TStaticArray<MR::ITextureSettings*> _MR_TEXTURE_BIND_TARGETS_SAMPLERS_;

class _MR_TEXTURES_BIND_TARGETS_NULL_ {
public:
    _MR_TEXTURES_BIND_TARGETS_NULL_() {
        int units = MR::MachineInfo::MaxActivedTextureUnits();
        Assert(units < 4)
        _MR_TEXTURE_BIND_TARGETS_ = MR::TStaticArray<MR::ITexture*>(new MR::ITexture*[units], units, true);
        _MR_TEXTURE_BIND_TARGETS_SAMPLERS_ = MR::TStaticArray<MR::ITextureSettings*>(new MR::ITextureSettings*[units], units, true);
        for(size_t i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
            _MR_TEXTURE_BIND_TARGETS_.GetRaw()[i] = nullptr;
            _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetRaw()[i] = nullptr;
        }
    }
};
_MR_TEXTURES_BIND_TARGETS_NULL_ _MR_TEXTURES_BIND_TARGETS_NULL_DEF_;

unsigned int _MR_TEXTURE_TYPE_TO_GL_TARGET_[]{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

namespace MR {

void Texture::Bind(unsigned short const& unit) {
    Assert(GetGPUHandle() == 0)
    Assert(unit >= _MR_TEXTURE_BIND_TARGETS_.GetNum())

    if(_MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit] == dynamic_cast<MR::ITexture*>(this)) return;
    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], _handle);
    } else {
        int actived_tex = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], _handle);
        glActiveTexture(actived_tex);
    }

    MR::ITextureSettings* ts = GetSettings();
    _MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit] = dynamic_cast<MR::ITexture*>(this);
    _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetRaw()[unit] = ts;
    glBindSampler(unit, (ts) ? ts->GetGPUHandle() : 0);
}

unsigned short Texture::Bind() {
    for(unsigned short i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
        if(_MR_TEXTURE_BIND_TARGETS_.GetRaw()[i] == nullptr) {
            Bind((unsigned short)i);
            return i;
        }
    }
    return 0;
}

ITexture* Texture::ReBind(unsigned short const& unit) {
    ITexture* t = TextureGetBinded(unit);
    this->Bind(unit);
    return t;
}

void Texture::Create(ITexture::Types const& type) {
    if(GetGPUHandle() != 0) {
        Destroy();
    }
    glGenTextures(1, &_handle);
    _texture_type = type;
}

void Texture::GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataTypes& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) {
    Assert(_handle == 0)
    Assert(dstBufferSize == 0)
    Assert(!dstBuffer)

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glGetTextureImage(_handle, mipMapLevel, (int)dformat, (int)dtype, dstBufferSize, dstBuffer);
    } else {
        unsigned short fu = TextureFreeUnit();
        ITexture* binded = nullptr;
        if(fu == 0) binded = ReBind(0);
        else Bind(fu);
        glGetTexImage(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)dformat, (int)dtype, dstBuffer);
        if(fu == 0) binded->Bind(0);
        else TextureUnBind(fu, false);
    }
}

void MR::Texture::SetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataTypes& dtype, const ITexture::StorageDataFormat& sdFormat,
                         const int& width, const int& height, const int& depth,
                         void* data) {
    Assert(_handle == 0)
    Assert(width <= 0)
    Assert(!data)

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        switch(_texture_type) {
        case Base1D:
            glTextureImage1DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, 0, (int)dformat, (int)dtype, data);
            break;
        case Base2D:
            glTextureImage2DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, height, 0, (int)dformat, (int)dtype, data);
            break;
        case Base3D:
            glTextureImage3DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, height, depth, 0, (int)dformat, (int)dtype, data);
            break;
        }
    } else {
        unsigned short fu = TextureFreeUnit();
        ITexture* binded = nullptr;
        if(fu == 0) binded = ReBind(0);
        else Bind(fu);

        switch(_texture_type) {
        case Base1D:
            glTexImage1D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTexImage2D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, height, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTexImage3D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)sdFormat, width, height, depth, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }

        if(fu == 0) binded->Bind(0);
        else TextureUnBind(fu, false);
    }
}

void Texture::UpdateData(const int& mipMapLevel,
                            const int& xOffset, const int& yOffset, const int& zOffset,
                            const int& width, const int& height, const int& depth,
                            const ITexture::DataFormat& dformat, const ITexture::DataTypes& dtype,
                            void* data) {
    Assert(_handle == 0)
    Assert(width <= 0)
    Assert(xOffset <= 0)
    Assert(!data)

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        switch(_texture_type) {
        case Base1D:
            glTextureSubImage1D(_handle, mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTextureSubImage2D(_handle, mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTextureSubImage3D(_handle, mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }
    } else {
        unsigned short fu = TextureFreeUnit();
        ITexture* binded = nullptr;
        if(fu == 0) binded = ReBind(0);
        else Bind(fu);

        switch(_texture_type) {
        case Base1D:
            glTexSubImage1D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTexSubImage2D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTexSubImage3D(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }

        if(fu == 0) binded->Bind(0);
        else TextureUnBind(fu, false);
    }
}

void MR::Texture::ResetInfo() {
    /*if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_WIDTH, (int*)(&_width));
    } else {
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (int*)(&gl_width));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (int*)(&gl_height));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, (int*)(&gl_depth));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, (int*)(&gl_internal_format));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_RED_SIZE, (int*)(&gl_red_bits_num));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_GREEN_SIZE, (int*)(&gl_green_bits_num));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_BLUE_SIZE, (int*)(&gl_blue_bits_num));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH_SIZE, (int*)(&gl_depth_bits_num));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_ALPHA_SIZE, (int*)(&gl_alpha_bits_num));
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, (int*)(&gl_compressed));
    }
    gl_mem_image_size = ((gl_red_bits_num+gl_green_bits_num+gl_blue_bits_num+gl_depth_bits_num+gl_alpha_bits_num)/8)*gl_width*gl_height;*/
}

void MR::Texture::Destroy() {
    if(_handle != 0) {
        glDeleteTextures(1, &_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<MR::GPUObjectHandle*>(this), 0);
        OnDestroy(dynamic_cast<MR::ObjectHandle*>(this));
    }
}

MR::Texture::Texture() /*: _mem_size(0), _internal_format(0), _compressed(false), _settings(nullptr), _compression_mode(NoCompression), _texture_type(Base1D)*/ {
}

MR::Texture::~Texture() {
}

ITexture* TextureGetBinded(const unsigned short& unit) {
    Assert(unit >= _MR_TEXTURE_BIND_TARGETS_.GetNum())
    return _MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit];
}

void TextureUnBind(const unsigned short& unit, const bool& fast) {
    Assert(unit >= _MR_TEXTURE_BIND_TARGETS_.GetNum())
    if(_MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit] == nullptr) return;

    if(!fast) {
        if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            glBindMultiTextureEXT(GL_TEXTURE0+unit, _MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit]->GetType(), 0);
        } else {
            int actived_tex = 0;
            glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
            glActiveTexture(GL_TEXTURE0+unit);
            glBindTexture(_MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit]->GetType(), 0);
            glActiveTexture(actived_tex);
        }
        glBindSampler(unit, 0);
    }

    _MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit] = nullptr;
    _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetRaw()[unit] = nullptr;
}

unsigned short TextureFreeUnit() {
    for(unsigned short i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
        if(_MR_TEXTURE_BIND_TARGETS_.GetRaw()[i] == nullptr) {
            return i;
        }
    }
    return 0;
}

}
