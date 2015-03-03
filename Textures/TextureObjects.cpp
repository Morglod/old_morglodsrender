#include "TextureObjects.hpp"
#include "../Utils/Exception.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../Buffers/BuffersInterfaces.hpp"
#include "../StateCache.hpp"
#include "../Utils/Containers.hpp"

#include <SOIL.h>
#ifndef __glew_h__
#   include <GL\glew.h>
#endif

mu::ArrayHandle<mr::ITexture*> _MR_TEXTURE_BIND_TARGETS_;
mu::ArrayHandle<mr::ITextureSettings*> _MR_TEXTURE_BIND_TARGETS_SAMPLERS_;

mr::TDynamicArray<mr::ITexture*> _MR_REGISTERED_TEXTURES_;

bool __mr_TexturesIsInit = false;
void __mr_TexturesInit() {
    if(__mr_TexturesIsInit) return;
    int units = mr::gl::GetMaxTextureUnits();
    Assert(units >= 4);
    _MR_TEXTURE_BIND_TARGETS_ = mu::ArrayHandle<mr::ITexture*>(new mr::ITexture*[units], units, true);
    _MR_TEXTURE_BIND_TARGETS_SAMPLERS_ = mu::ArrayHandle<mr::ITextureSettings*>(new mr::ITextureSettings*[units], units, true);
    for(size_t i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
        _MR_TEXTURE_BIND_TARGETS_.GetArray()[i] = nullptr;
        _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetArray()[i] = nullptr;
    }
    __mr_TexturesIsInit = true;
}

#define __MR_REQUEST_TEXTURE_INIT() \
if(!__mr_TexturesIsInit) __mr_TexturesInit()

unsigned int _MR_TEXTURE_TYPE_TO_GL_TARGET_[]{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

namespace mr {

void Texture::Bind(unsigned short const& unit) {
    __MR_REQUEST_TEXTURE_INIT();
    Assert(GetGPUHandle() != 0);
    Assert(unit < _MR_TEXTURE_BIND_TARGETS_.GetNum());

    if(_MR_TEXTURE_BIND_TARGETS_.GetArray()[unit] == dynamic_cast<mr::ITexture*>(this)) return;

    if(mr::gl::IsOpenGL45()) {
        glBindTextureUnit(unit, _handle);
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
        glBindMultiTextureEXT(GL_TEXTURE0+unit, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], _handle);
    } else {
        int actived_tex = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
        glActiveTexture(GL_TEXTURE0+unit);
        glBindTexture(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], _handle);
        glActiveTexture(actived_tex);
    }

    mr::ITextureSettings* ts = GetSettings();
    _MR_TEXTURE_BIND_TARGETS_.GetArray()[unit] = dynamic_cast<mr::ITexture*>(this);
    _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetArray()[unit] = ts;
    glBindSampler(unit, (ts) ? ts->GetGPUHandle() : 0);
}

unsigned short Texture::Bind() {
    __MR_REQUEST_TEXTURE_INIT();
    for(unsigned short i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
        if(_MR_TEXTURE_BIND_TARGETS_.GetArray()[i] == nullptr) {
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

    if(mr::gl::IsOpenGL45()) glCreateTextures(type, 1, &_handle);
    else glGenTextures(1, &_handle);
    _texture_type = type;

    if(mr::gl::IsOpenGL45()) {
        glTextureParameteri(_handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(_handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
        glTextureParameteriEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteriEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteriEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteriEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        ITexture* tex = ReBind(0);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        if(tex) tex->Bind(0);
    }
}

void Texture::GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) {
    Assert(GetGPUHandle() != 0);
    Assert(dstBufferSize != 0);
    Assert(dstBuffer != nullptr);

    if(mr::gl::IsOpenGL45()) {
        glGetTextureImage(_handle, mipMapLevel, (int)dformat, (int)dtype, dstBufferSize, dstBuffer);
    } else if(mr::gl::IsDirectStateAccessSupported()) {
        glGetTextureImageEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, (int)dformat, (int)dtype, dstBuffer);
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

void mr::Texture::SetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, const ITexture::StorageDataFormat& sdFormat,
                         const int& width, const int& height, const int& depth,
                         void* data) {
    Assert(GetGPUHandle() != 0);
    Assert(width > 0);
    Assert(data != nullptr);

    if(mr::gl::IsDirectStateAccessSupported()) {
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

bool Texture::UpdateData(const int& mipMapLevel,
                            const int& xOffset, const int& yOffset, const int& zOffset,
                            const int& width, const int& height, const int& depth,
                            const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                            void* data) {
    Assert(GetGPUHandle() != 0);
    Assert(width > 0);
    Assert(xOffset > 0);

    if(mr::gl::IsOpenGL45()) {
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
    } else if(mr::gl::IsDirectStateAccessSupported()) {
        switch(_texture_type) {
        case Base1D:
            glTextureSubImage1DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTextureSubImage2DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTextureSubImage3DEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
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

    return true;
}

bool mr::Texture::UpdateDataFromBuffer(const int& mipMapLevel,
                            const int& xOffset, const int& yOffset, const int& zOffset,
                            const int& width, const int& height, const int& depth,
                            const ITexture::DataFormat& dformat, const ITexture::DataType& dtype,
                            IGPUBuffer* buffer)
{
    StateCache* stateCache = StateCache::GetDefault();
    IGPUBuffer* binded = nullptr;
    if(!stateCache->ReBindBuffer(buffer, IGPUBuffer::PixelUnpackBuffer, &binded)) {
        mr::Log::LogString("Bind buffer failed in Texture::UpdateDataFromBuffer.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    bool b = this->UpdateData(mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, dformat, dtype, 0);
    if(binded) stateCache->BindBuffer(binded, IGPUBuffer::PixelUnpackBuffer);

    return b;
}

bool mr::Texture::Complete(bool mipMaps) {
    if(_handle == 0) {
        mr::Log::LogString("Failed Texture::Complete. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    _mipmaps = mipMaps;
    if(mipMaps) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    mr::MachineInfo::ClearError();
#endif
        if(mr::gl::IsOpenGL45()) {
            glGenerateTextureMipmap(_handle);
        } else if(mr::gl::IsDirectStateAccessSupported()) {
            glGenerateTextureMipmapEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type]);
        } else {
            ITexture* tex = ReBind(0);
            glGenerateMipmap(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type]);
            if(tex) tex->Bind(0);
        }
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::MachineInfo::CatchError(0, &gl_er)) {
        mr::Log::LogString("Error in Texture::Complete : glGenerateMipmap() ended with \"" + std::to_string(gl_er) + "\" code. ", MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif
    }

    UpdateInfo();

    return true;
}

void Texture::UpdateInfo() {
    if(_handle == 0) {
        _sizes = mu::ArrayHandle<TextureSizeInfo>();
        _bits = TextureBitsInfo(0,0,0,0,0);
        _mem_size = 0;
        _compressed = false;
        return;
    }

    {
        ITexture* binded = nullptr;
        if((mr::gl::IsOpenGL45() || mr::gl::IsDirectStateAccessSupported()) == false) binded = ReBind(0);

        int maxW = 0, maxH = 0, maxD = 0;
        if(mr::gl::IsOpenGL45()) {
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_DEPTH, &maxD);
        } else if(mr::gl::IsDirectStateAccessSupported()) {
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_DEPTH, &maxD);
        } else {
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTexLevelParameteriv(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_DEPTH, &maxD);
        }

        int numMipMaps = (_mipmaps) ? (1 + glm::floor(glm::log2( (double)glm::max(glm::max(maxW, maxH), maxD) ))) : 1;
        _sizes = mu::ArrayHandle<TextureSizeInfo>(new TextureSizeInfo[numMipMaps], numMipMaps, true);
        TextureSizeInfo* szAr = _sizes.GetArray();

        int fi = 0, rs = 0, gs = 0, bs = 0, ds = 0, as = 0, cm = 0;
        if(mr::gl::IsOpenGL45()) {
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameteriv(_handle, 0, GL_TEXTURE_COMPRESSED, &cm);
        } else if(mr::gl::IsDirectStateAccessSupported()) {
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], 0, GL_TEXTURE_COMPRESSED, &cm);
        } else {
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

            if(mr::gl::IsOpenGL45()) {
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameteriv(_handle, i, GL_TEXTURE_DEPTH, &d);
            } else if(mr::gl::IsDirectStateAccessSupported()) {
                glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameterivEXT(_handle, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_texture_type], i, GL_TEXTURE_DEPTH, &d);
            } else {
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

void mr::Texture::Destroy() {
    if(_handle != 0) {
        glDeleteTextures(1, &_handle);
        _handle = 0;
        OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
    }
}

mr::Texture::Texture() {
    _MR_REGISTERED_TEXTURES_.PushBack(dynamic_cast<ITexture*>(this));
}

mr::Texture::~Texture() {
    _MR_REGISTERED_TEXTURES_.Erase(dynamic_cast<ITexture*>(this));
}

void mr::TextureList::Bind() {
    size_t num = _textures.GetNum();
    if(GLEW_ARB_multi_bind) {
        glBindTextures(_firstUnit, num, _gpuHandles.GetArray());
        glBindSamplers(_firstUnit, num, &(_gpuHandles.GetArray()[num]));
    } else {
        for(size_t i = 0; i < num; ++i) {
            _textures.GetArray()[i]->Bind(i + _firstUnit);
        }
    }
}

void mr::TextureList::SetTexture(ITexture* tex, unsigned short const& index) {
    size_t num = _textures.GetNum();
    Assert(index < num)
    _textures.GetArray()[index] = tex;
    if(tex == nullptr) {
        _gpuHandles.GetArray()[index] = _gpuHandles.GetArray()[index + num] = 0;
    }
    else {
        _gpuHandles.GetArray()[index] = tex->GetGPUHandle();
        _gpuHandles.GetArray()[index + num] = (tex->GetSettings() != nullptr) ? tex->GetSettings()->GetGPUHandle() : 0;
    }
}

void mr::TextureList::SetTextures(mu::ArrayHandle<ITexture*> tex) {
    _textures = tex;
    size_t texNum = tex.GetNum();
    if(texNum != 0) {
        unsigned int* handles = new unsigned int [texNum * 2];
        for(size_t i = 0; i < texNum; ++i) {
            handles[i] = tex.GetArray()[i]->GetGPUHandle();
            auto texSampler = tex.GetArray()[i]->GetSettings();
            handles[i + texNum] = (texSampler != nullptr) ? texSampler->GetGPUHandle() : 0;
        }
        _gpuHandles = mu::ArrayHandle<unsigned int>(handles, texNum * 2);
    } else {
        _gpuHandles = mu::ArrayHandle<unsigned int>();
    }
}

ITexture* TextureGetBinded(const unsigned short& unit) {
    __MR_REQUEST_TEXTURE_INIT();
    Assert(unit < _MR_TEXTURE_BIND_TARGETS_.GetNum());
    return _MR_TEXTURE_BIND_TARGETS_.GetArray()[unit];
}

void TextureUnBind(const unsigned short& unit, const bool& fast) {
    __MR_REQUEST_TEXTURE_INIT();
    Assert(unit < _MR_TEXTURE_BIND_TARGETS_.GetNum());
    if(_MR_TEXTURE_BIND_TARGETS_.GetArray()[unit] == nullptr) return;

    if(!fast) {
        if(mr::gl::IsOpenGL45()) {
            glBindTextureUnit(unit, 0);
        }
        else if(mr::gl::IsDirectStateAccessSupported()) {
            glBindMultiTextureEXT(GL_TEXTURE0+unit, _MR_TEXTURE_TYPE_TO_GL_TARGET_[_MR_TEXTURE_BIND_TARGETS_.GetArray()[unit]->GetType()], 0);
        } else {
            int actived_tex = 0;
            glGetIntegerv(GL_ACTIVE_TEXTURE, &actived_tex);
            glActiveTexture(GL_TEXTURE0+unit);
            glBindTexture(_MR_TEXTURE_TYPE_TO_GL_TARGET_[_MR_TEXTURE_BIND_TARGETS_.GetArray()[unit]->GetType()], 0);
            glActiveTexture(actived_tex);
        }
        glBindSampler(unit, 0);
    }

    _MR_TEXTURE_BIND_TARGETS_.GetArray()[unit] = nullptr;
    _MR_TEXTURE_BIND_TARGETS_SAMPLERS_.GetArray()[unit] = nullptr;
}

unsigned short TextureFreeUnit() {
    __MR_REQUEST_TEXTURE_INIT();
    for(unsigned short i = 0; i < _MR_TEXTURE_BIND_TARGETS_.GetNum(); ++i){
        if(_MR_TEXTURE_BIND_TARGETS_.GetArray()[i] == nullptr) {
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
	tex->_texture_type = mr::ITexture::Base2D;
	tex->Complete(true);
	return dynamic_cast<mr::ITexture*>(tex);
}

float * __MR_CHECKER_NEW_IMAGE_(size_t const& sizes, float c) {
    float * pixel_data = new float [sizes*sizes];
    for(size_t i = 0; i < (sizes*sizes); ++i){
        pixel_data[i] = c;
    }
    return pixel_data;
}

ITexture* Texture::CreateMipmapChecker() {
    ITexture* tex = dynamic_cast<ITexture*>(new mr::Texture());
    tex->Create(Base2D);

    if(!tex->IsGood()) {
        delete tex;
        return nullptr;
    }

    for(int i = 0; i < 10; ++i) {
        int sz = glm::pow(2, 10-i);
        float* data = __MR_CHECKER_NEW_IMAGE_(sz, (float)(10-i) * 0.1f);
        mr::Log::LogString(std::to_string((float)(10-i) * 0.1f));
        tex->SetData(i, ITexture::DF_RED, ITexture::DT_FLOAT, ITexture::SDF_RGB, sz, sz, 0, &data[0]);
        delete [] data;
    }

    tex->Complete(false);

    return tex;
}

void DestroyAllTextures() {
    for(size_t i = 0; i < _MR_REGISTERED_TEXTURES_.GetNum(); ++i) {
        _MR_REGISTERED_TEXTURES_.At(i)->Destroy();
    }
}

}
