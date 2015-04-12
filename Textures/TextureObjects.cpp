#include "TextureObjects.hpp"
#include "../Utils/Exception.hpp"
#include "../Utils/Log.hpp"
//#include "../RenderManager.hpp"
#include "../Utils/Debug.hpp"
#include "../MachineInfo.hpp"
#include "../Buffers/BuffersInterfaces.hpp"
#include "../StateCache.hpp"
#include "../Utils/Containers.hpp"
#include "../StateCache.hpp"

#include <SOIL.h>
#ifndef __glew_h__
#   include <GL\glew.h>
#endif

mr::TDynamicArray<mr::ITexture*> _MR_REGISTERED_TEXTURES_;

unsigned int MR_TEXTURE_TARGET2[]{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D
};

namespace mr {

bool Texture::Create(ITexture::Types const& type) {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        Destroy();
    }

    if(mr::gl::IsOpenGL45()) glCreateTextures(type, 1, &handle);
    else glGenTextures(1, &handle);
    _texture_type = type;

    if(mr::gl::IsOpenGL45()) {
        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else if(GLEW_EXT_direct_state_access) {
        glTextureParameteriEXT(handle, MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteriEXT(handle, MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteriEXT(handle, MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteriEXT(handle, MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        ITexture* binded = nullptr;
        if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
            mr::Log::LogString("Bind texture failed in Texture::Create.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glTexParameteri(MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(MR_TEXTURE_TARGET2[_texture_type], GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        if(binded) stateCache->BindTexture(binded, 0);
    }

    return true;
}

bool Texture::GetData(const int& mipMapLevel,
                const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, unsigned int const& dstBufferSize,
                void* dstBuffer) {
    Assert(GetGPUHandle() != 0);
    Assert(dstBufferSize != 0);
    Assert(dstBuffer != nullptr);

    if(mr::gl::IsOpenGL45()) {
        glGetTextureImage(GetGPUHandle(), mipMapLevel, (int)dformat, (int)dtype, dstBufferSize, dstBuffer);
    } else if(GLEW_EXT_direct_state_access) {
        glGetTextureImageEXT(GetGPUHandle(), MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)dformat, (int)dtype, dstBuffer);
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        ITexture* binded = nullptr;
        if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
            mr::Log::LogString("Bind texture failed in Texture::GetData.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glGetTexImage(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)dformat, (int)dtype, dstBuffer);

        if(binded) stateCache->BindTexture(binded, 0);
    }

    return true;
}

bool mr::Texture::SetData(const int& mipMapLevel,
                         const ITexture::DataFormat& dformat, const ITexture::DataType& dtype, const ITexture::StorageDataFormat& sdFormat,
                         const int& width, const int& height, const int& depth,
                         void* data) {
    Assert(GetGPUHandle() != 0);
    Assert(width > 0);
    Assert(data != nullptr);


    if(GLEW_EXT_direct_state_access) {
        unsigned int handle = GetGPUHandle();
        glPushClientAttribDefaultEXT(GL_CLIENT_PIXEL_STORE_BIT);
        switch(_texture_type) {
        case Base1D:
            glTextureImage1DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, 0, (int)dformat, (int)dtype, data);
            break;
        case Base2D:
            glTextureImage2DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, height, 0, (int)dformat, (int)dtype, data);
            break;
        case Base3D:
            glTextureImage3DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, height, depth, 0, (int)dformat, (int)dtype, data);
            break;
        }
        glPopClientAttrib();
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        ITexture* binded = nullptr;
        if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
            mr::Log::LogString("Bind texture failed in Texture::SetData.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        switch(_texture_type) {
        case Base1D:
            glTexImage1D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTexImage2D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, height, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTexImage3D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, (int)sdFormat, width, height, depth, 0, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }

        if(binded) stateCache->BindTexture(binded, 0);
    }

    return true;
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
        unsigned int handle = GetGPUHandle();
        switch(_texture_type) {
        case Base1D:
            glTextureSubImage1D(handle, mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTextureSubImage2D(handle, mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTextureSubImage3D(handle, mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }
    } else if(GLEW_EXT_direct_state_access) {
        unsigned int handle = GetGPUHandle();
        switch(_texture_type) {
        case Base1D:
            glTextureSubImage1DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTextureSubImage2DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTextureSubImage3DEXT(handle, MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        ITexture* binded = nullptr;
        if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
            mr::Log::LogString("Bind texture failed in Texture::UpdateData.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        switch(_texture_type) {
        case Base1D:
            glTexSubImage1D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, width, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base2D:
            glTexSubImage2D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, yOffset, width, height, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        case Base3D:
            glTexSubImage3D(MR_TEXTURE_TARGET2[_texture_type], mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, (unsigned int)dformat, (unsigned int)dtype, data);
            break;
        }

        if(binded) stateCache->BindTexture(binded, 0);
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
    if(!stateCache->ReBindBuffer(buffer, StateCache::PixelUnpackBuffer, &binded)) {
        mr::Log::LogString("Bind buffer failed in Texture::UpdateDataFromBuffer.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    const bool b = this->UpdateData(mipMapLevel, xOffset, yOffset, zOffset, width, height, depth, dformat, dtype, 0);
    if(binded) stateCache->BindBuffer(binded, StateCache::PixelUnpackBuffer);

    return b;
}

bool mr::Texture::Complete(bool mipMaps) {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
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
            glGenerateTextureMipmap(handle);
        } else if(GLEW_EXT_direct_state_access) {
            glGenerateTextureMipmapEXT(handle, MR_TEXTURE_TARGET2[_texture_type]);
        } else {
            StateCache* stateCache = StateCache::GetDefault();
            ITexture* binded = nullptr;
            if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
                mr::Log::LogString("Bind texture failed in Texture::Complete.", MR_LOG_LEVEL_ERROR);
                return false;
            }

            glGenerateMipmap(MR_TEXTURE_TARGET2[_texture_type]);

            if(binded) stateCache->BindTexture(binded, 0);
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

bool Texture::UpdateInfo() {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        _sizes = mu::ArrayHandle<TextureSizeInfo>();
        _bits = TextureBitsInfo(0,0,0,0,0);
        _mem_size = 0;
        _compressed = false;
        return false;
    }

    {
        StateCache* stateCache = StateCache::GetDefault();
        ITexture* binded = nullptr;

        if((mr::gl::IsOpenGL45() || GLEW_EXT_direct_state_access) == false) {
            if(!stateCache->ReBindTexture(dynamic_cast<ITexture*>(this), 0, &binded)) {
                mr::Log::LogString("Bind texture failed in Texture::UpdateInfo.", MR_LOG_LEVEL_ERROR);
                return false;
            }
        }

        int maxW = 0, maxH = 0, maxD = 0;
        if(mr::gl::IsOpenGL45()) {
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_DEPTH, &maxD);
        } else if(GLEW_EXT_direct_state_access) {
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_DEPTH, &maxD);
        } else {
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_DEPTH, &maxD);
        }

        const int numMipMaps = (_mipmaps) ? (1 + glm::floor(glm::log2( (double)glm::max(glm::max(maxW, maxH), maxD) ))) : 1;
        _sizes = mu::ArrayHandle<TextureSizeInfo>(new TextureSizeInfo[numMipMaps], numMipMaps, true);
        TextureSizeInfo* szAr = _sizes.GetArray();

        int fi = 0, rs = 0, gs = 0, bs = 0, ds = 0, as = 0, cm = 0;
        if(mr::gl::IsOpenGL45()) {
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_COMPRESSED, &cm);
        } else if(GLEW_EXT_direct_state_access) {
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_COMPRESSED, &cm);
        } else {
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], 0, GL_TEXTURE_COMPRESSED, &cm);
        }

        _bits = TextureBitsInfo(rs, gs, bs, ds, as);
        _compressed = (GL_TRUE == cm);
        _mem_size = 0;

        for(int i = 0; i < numMipMaps; ++i) {
            int w = 0, h = 0, d = 0;

            if(mr::gl::IsOpenGL45()) {
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_DEPTH, &d);
            } else if(GLEW_EXT_direct_state_access) {
                glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameterivEXT(handle, MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_DEPTH, &d);
            } else {
                glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_WIDTH, &w);
                glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_HEIGHT, &h);
                glGetTexLevelParameteriv(MR_TEXTURE_TARGET2[_texture_type], i, GL_TEXTURE_DEPTH, &d);
            }

            szAr[i] = TextureSizeInfo(w, h, d);
            _mem_size += ((rs+gs+bs+ds+as)/8)*w*h*d;
        }

        if(binded) stateCache->BindTexture(binded, 0);
    }

    return true;
}

void mr::Texture::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteTextures(1, &handle);
        SetGPUHandle(0);
    }
}

mr::Texture::Texture() {
    _MR_REGISTERED_TEXTURES_.PushBack(dynamic_cast<ITexture*>(this));
}

mr::Texture::~Texture() {
    _MR_REGISTERED_TEXTURES_.Erase(dynamic_cast<ITexture*>(this));
}

void mr::TextureBindList::Bind() {
	//TODO, BindFunction in StateCache.

    const size_t num = _textures.GetNum();
	const unsigned int* handles = _gpuHandles.GetArray();
    if(GLEW_ARB_multi_bind) {
        glBindTextures(_firstUnit, num, handles);
        glBindSamplers(_firstUnit, num, &(handles[num]));
    } else {
        ITexture** texArray = _textures.GetArray();
        StateCache* stateCache = StateCache::GetDefault();
        for(size_t i = 0; i < num; ++i) {
            if(!stateCache->BindTexture(texArray[i], i + _firstUnit)) {
                mr::Log::LogString("Bind texture failed in TextureBindList::Bind.", MR_LOG_LEVEL_ERROR);
            }
        }
    }
}

void mr::TextureBindList::SetTexture(ITexture* tex, unsigned short const& index) {
    const size_t num = _textures.GetNum();
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

void mr::TextureBindList::SetTextures(mu::ArrayHandle<ITexture*> tex) {
    _textures = tex;
    const size_t texNum = tex.GetNum();
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

ITexture* Texture::FromFile(std::string const& path) {
    Texture* tex = new Texture();

	const unsigned int handle =
		SOIL_load_OGL_texture(	path.c_str(),
                               SOIL_LOAD_AUTO,
                               SOIL_CREATE_NEW_ID,
                               SOIL_FLAG_MIPMAPS);

	if(handle == 0) return nullptr;
	tex->SetGPUHandle(handle);
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

    if(tex->GetGPUHandle() == 0) {
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
