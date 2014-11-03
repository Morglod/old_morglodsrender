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

    /*
    STUPID EXTENSIONS, THIS METHODS NOT SUPPORTED
    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {*/
        ITexture* tex = ReBind(0);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(tex) tex->Bind(0);
    //}
}

void Texture::GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataTypes& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) {
    Assert(_handle == 0)
    Assert(dstBufferSize == 0)
    Assert(!dstBuffer)

    /*if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glGetTextureImage(_handle, mipMapLevel, (int)dformat, (int)dtype, dstBufferSize, dstBuffer);
    } else {*/
        unsigned short fu = TextureFreeUnit();
        ITexture* binded = nullptr;
        if(fu == 0) binded = ReBind(0);
        else Bind(fu);
        glGetTexImage(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)dformat, (int)dtype, dstBuffer);
        if(fu == 0) binded->Bind(0);
        else TextureUnBind(fu, false);
    //}
}

void MR::Texture::SetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataTypes& dtype, const ITexture::StorageDataFormat& sdFormat,
                         const int& width, const int& height, const int& depth,
                         void* data) {
    Assert(_handle == 0)
    Assert(width <= 0)
    Assert(!data)

    if(MR::MachineInfo::IsDirectStateAccessSupported()) {
        glPushClientAttribDefaultEXT(GL_CLIENT_PIXEL_STORE_BIT);
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
        glPopClientAttrib();
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

    /*if(MR::MachineInfo::IsDirectStateAccessSupported()) {
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
    } else {*/
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
    //}
}

bool MR::Texture::Complete(bool mipMaps) {
    if(_handle == 0) {
        MR::Log::LogString("Failed Texture::Complete. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    _mipmaps = mipMaps;
    if(mipMaps) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    MR::MachineInfo::ClearError();
#endif
        /*if(MR::MachineInfo::IsDirectStateAccessSupported()) {
            glGenerateTextureMipmap(_handle);
        } else {*/
            ITexture* tex = ReBind(0);
            glGenerateMipmap(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type]);
            if(tex) tex->Bind(0);
        //}
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(MR::MachineInfo::CatchError(0, &gl_er)) {
        MR::Log::LogString("Error in Texture::Complete : glGenerateMipmap() ended with \"" + std::to_string(gl_er) + "\" code. ", MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif
    }

    UpdateInfo();

    return true;
}

void Texture::UpdateInfo() {
    if(_handle == 0) {
        _sizes = TStaticArray<TextureSizeInfo>();
        _bits = TextureBitsInfo(0,0,0,0,0);
        _mem_size = 0;
        _compressed = false;
        return;
    }

    {
        ITexture* binded = nullptr;
        if(MR::MachineInfo::gl_version_over_4_5() == false) binded = ReBind(0);

        int maxW = 0, maxH = 0, maxD = 0;
        if(MR::MachineInfo::gl_version_over_4_5()) {
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_DEPTH, &maxD);
        }
        else {
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_DEPTH, &maxD);
        }

        int numMipMaps = (_mipmaps) ? (1 + glm::floor(glm::log2( (double)glm::max(glm::max(maxW, maxH), maxD) ))) : 1;
        _sizes = TStaticArray<TextureSizeInfo>(numMipMaps);
        TextureSizeInfo* szAr = _sizes.GetRaw();

        int fi = 0, rs = 0, gs = 0, bs = 0, ds = 0, as = 0, cm = 0;
        if(MR::MachineInfo::gl_version_over_4_5()) {
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_COMPRESSED, &cm);
        }
        else {
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_COMPRESSED, &cm);
        }

        _bits = TextureBitsInfo(rs, gs, bs, ds, as);
        _compressed = (GL_TRUE == cm);
        _mem_size = 0;

        for(int i = 0; i < numMipMaps; ++i) {
            int w = 0, h = 0, d = 0;

            if(MR::MachineInfo::gl_version_over_4_5()) {
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_DEPTH, &d);
            }
            else {
                glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_WIDTH, &w);
                glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_HEIGHT, &h);
                glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_DEPTH, &d);
            }

            szAr[i] = TextureSizeInfo(w, h, d);
            _mem_size += ((rs+gs+bs+ds+as)/8)*w*h*d;
        }

        if(binded) binded->Bind(0);
    }
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
            glBindMultiTextureEXT(GL_TEXTURE0+unit, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit]->GetType()], 0);
        } else {
            int actived_tex = 0;
            glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
            glActiveTexture(GL_TEXTURE0+unit);
            glBindTexture(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_MR_TEXTURE_BIND_TARGETS_.GetRaw()[unit]->GetType()], 0);
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

ITexture* Texture::FromFile(std::string const& path) {
    Texture* tex = new Texture();
    tex->_handle =
        SOIL_load_OGL_texture(path.c_str(),
                               SOIL_LOAD_AUTO,
                               SOIL_CREATE_NEW_ID,
                               SOIL_FLAG_MIPMAPS);
	if(tex->_handle == 0) return nullptr;
	tex->_texture_type = MR::ITexture::Base2D;
	tex->Complete(true);
	return dynamic_cast<MR::ITexture*>(tex);
}

float * __MR_CHECKER_NEW_IMAGE_(size_t const& sizes, float c) {
    float * pixel_data = new float [sizes*sizes];
    for(size_t i = 0; i < (sizes*sizes); ++i){
        pixel_data[i] = c;
    }
    return pixel_data;
}

ITexture* Texture::CreateMipmapChecker() {
    ITexture* tex = dynamic_cast<ITexture*>(new MR::Texture());
    tex->Create(Base2D);

    if(!tex->Good()) {
        delete tex;
        return nullptr;
    }
    for(int i = 0; i < 10; ++i) {
        int sz = glm::pow(2, 10-i);
        float* data = __MR_CHECKER_NEW_IMAGE_(sz, (float)(10-i) * 0.1f);
        MR::Log::LogString(std::to_string((float)(10-i) * 0.1f));
        tex->SetData(i, ITexture::DF_RED, ITexture::DT_FLOAT, ITexture::SDF_RGB, sz, sz, 0, &data[0]);
        delete [] data;
    }

    tex->Complete(false);

    return tex;
}

}
