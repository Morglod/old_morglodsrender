#include "Texture.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Log.hpp"
#include "../StateCache.hpp"
#include "../Utils/Debug.hpp"

#ifndef __glew_h__
#   include <GL\glew.h>
#endif

namespace mr {

TextureSizeInfo::TextureSizeInfo() : width(0), height(0), depth(0) {}
TextureSizeInfo::TextureSizeInfo(unsigned short const& w, unsigned short const& h, unsigned short const& d)
    : width(w), height(h), depth(d) {}
TextureSizeInfo::TextureSizeInfo(glm::ivec3 const& s)
    : width(s.x), height(s.y), depth(s.z) {}

TextureBitsInfo::TextureBitsInfo() : r(0), g(0), b(0), d(0), a(0) {}
TextureBitsInfo::TextureBitsInfo(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char d_, unsigned char a_)
    : r(r_), g(g_), b(b_), d(d_), a(a_) {}

bool Texture::UpdateInfo() {
    unsigned int handle = GetGPUHandle();

    if(handle == 0) {
        _sizes = mu::ArrayHandle<TextureSizeInfo>();
        _bits = TextureBitsInfo(0,0,0,0,0);
        _memSize = 0;
        _compression = NoCompression;
        _type = Type_None;
        _storageDataFormat = SDF_NONE;
        _dataFormat = DF_NONE;
        _dataType = DT_NONE;
        _residentHandle = 0;
        mr::Log::LogString("Failed Texture::UpdateInfo. handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    unsigned int texType = GetType();

    {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if((mr::gl::IsDSA_ARB() || mr::gl::IsDSA_EXT()) == false) {
            if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
                mr::Log::LogString("Failed in Texture::UpdateInfo. Failed bind texture.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            if(!stateCache->BindTextureNotCached(0, handle, texType)) {
                mr::Log::LogString("Failed in Texture::UpdateInfo. Failed bind texture.", MR_LOG_LEVEL_ERROR);
                return false;
            }
        }

        int maxW = 0, maxH = 0, maxD = 0;
        if(mr::gl::IsDSA_ARB()) {
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_DEPTH, &maxD);
        } else if(mr::gl::IsDSA_EXT()) {
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_DEPTH, &maxD);
        } else {
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_WIDTH, &maxW);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_HEIGHT, &maxH);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_DEPTH, &maxD);
        }

        const int numMipMaps = (_mipmaps) ? (1 + glm::floor(glm::log2( (double)glm::max(glm::max(maxW, maxH), maxD) ))) : 1;
        _sizes = mu::ArrayHandle<TextureSizeInfo>(new TextureSizeInfo[numMipMaps], numMipMaps, true);
        TextureSizeInfo* szAr = _sizes.GetArray();

        int fi = 0, rs = 0, gs = 0, bs = 0, ds = 0, as = 0, cm = 0;
        if(mr::gl::IsDSA_ARB()) {
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameteriv(handle, 0, GL_TEXTURE_COMPRESSED, &cm);
        } else if(mr::gl::IsDSA_EXT()) {
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTextureLevelParameterivEXT(handle, texType, 0, GL_TEXTURE_COMPRESSED, &cm);
        } else {
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_INTERNAL_FORMAT, &fi);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_RED_SIZE, &rs);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_GREEN_SIZE, &gs);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_BLUE_SIZE, &bs);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_DEPTH_SIZE, &ds);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_ALPHA_SIZE, &as);
            glGetTexLevelParameteriv(texType, 0, GL_TEXTURE_COMPRESSED, &cm);
        }

        _bits = TextureBitsInfo(rs, gs, bs, ds, as);

        for(int i = 0; i < numMipMaps; ++i) {
            int w = 0, h = 0, d = 0;
            if(mr::gl::IsDSA_ARB()) {
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameteriv(handle, i, GL_TEXTURE_DEPTH, &d);
            } else if(mr::gl::IsDSA_EXT()) {
                glGetTextureLevelParameterivEXT(handle, texType, i, GL_TEXTURE_WIDTH, &w);
                glGetTextureLevelParameterivEXT(handle, texType, i, GL_TEXTURE_HEIGHT, &h);
                glGetTextureLevelParameterivEXT(handle, texType, i, GL_TEXTURE_DEPTH, &d);
            } else {
                glGetTexLevelParameteriv(texType, i, GL_TEXTURE_WIDTH, &w);
                glGetTexLevelParameteriv(texType, i, GL_TEXTURE_HEIGHT, &h);
                glGetTexLevelParameteriv(texType, i, GL_TEXTURE_DEPTH, &d);
            }
            szAr[i] = TextureSizeInfo(w, h, d);
        }
        _CalcMemSize();
        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }

    return true;
}

bool Texture::Complete(bool mipMaps) {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed Texture::Complete. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    unsigned int texType = GetType();
    _mipmaps = mipMaps;
    if(mipMaps) {
#ifdef MR_CHECK_LARGE_GL_ERRORS
    int gl_er = 0;
    mr::gl::ClearError();
#endif
        if(mr::gl::IsDSA_ARB()) {
            glGenerateTextureMipmap(handle);
        } else if(mr::gl::IsDSA_EXT()) {
            glGenerateTextureMipmapEXT(handle, texType);
        } else {

            StateCache* stateCache = StateCache::GetDefault();
            unsigned int binded_gpu_handle = 0;
            unsigned int binded_tex_type = 0;

            if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
                mr::Log::LogString("Failed Texture::Complete. Failed bind texture.", MR_LOG_LEVEL_ERROR);
                return false;
            }
            if(!stateCache->BindTextureNotCached(0, handle, texType)) {
                mr::Log::LogString("Failed Texture::Complete. Failed bind texture.", MR_LOG_LEVEL_ERROR);
                return false;
            }

            glGenerateMipmap(texType);

            if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
        }
#ifdef MR_CHECK_LARGE_GL_ERRORS
    if(mr::gl::CheckError(0, &gl_er)) {
        mr::Log::LogString("Error in Texture::Complete : glGenerateMipmap() ended with \"" + std::to_string(gl_er) + "\" code. ", MR_LOG_LEVEL_ERROR);
        return false;
    }
#endif
    }

    UpdateInfo();

    return true;
}

bool Texture::GetData(  int const& mipMapLevel,
                        Texture::DataFormat const& dformat, Texture::DataType const& dtype, unsigned int const& dstBufferSize,
                        void* dstBuffer)
{
    Assert(dstBufferSize == 0);
    Assert(dstBuffer == nullptr);

    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed Texture::GetData. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    unsigned int texType = GetType();
    if(mr::gl::IsDSA_ARB()) {
        glGetTextureImage(handle, mipMapLevel, dformat, dtype, dstBufferSize, dstBuffer);
    } else if(mr::gl::IsDSA_EXT()) {
        glGetTextureImageEXT(handle, texType, mipMapLevel, dformat, dtype, dstBuffer);
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
            mr::Log::LogString("Failed Texture::GetData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(!stateCache->BindTextureNotCached(0, handle, texType)) {
            mr::Log::LogString("Failed Texture::GetData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glGetTexImage(texType, mipMapLevel, dformat, dtype, dstBuffer);

        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }
    return true;
}

bool Texture::MakeResident() {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed Texture::MakeResident. Handle is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    bool arb_bindless = false;
    if(mr::gl::IsBindlessTextureSupported(arb_bindless)) {
        if(arb_bindless) {
            _residentHandle = glGetTextureHandleARB(handle);
            glMakeTextureHandleResidentARB(_residentHandle);
        }
        else {
            _residentHandle = glGetTextureHandleNV(handle);
            glMakeTextureHandleResidentNV(_residentHandle);
        }
        return true;
    }
    return false;
}

void Texture::MakeNonResident() {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) {
        mr::Log::LogString("Failed Texture::MakeNonResident. Handle is null.", MR_LOG_LEVEL_ERROR);
        return;
    }

    bool arb_bindless = false;
    if(mr::gl::IsBindlessTextureSupported(arb_bindless)) {
        if(arb_bindless) glMakeTextureHandleNonResidentARB(_residentHandle);
        else glMakeTextureHandleNonResidentNV(_residentHandle);
    }
    _residentHandle = 0;
}

void Texture::Destroy() {
    MakeNonResident();
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteTextures(1, &handle);
        SetGPUHandle(0);
    }
}

void Texture::_CalcMemSize() {
    _memSize = 0;

    int mipMapsNum = GetMipMapLevelsNum();
    if(mipMapsNum == 0) {
        mr::Log::LogString("Failed Texture::_CalcMemSize. mipMapsNum is zero.", MR_LOG_LEVEL_WARNING);
        return;
    }

    TextureBitsInfo bitsInfo = GetBitsInfo();
    size_t texelBitsSize = bitsInfo.CalcTotalBits();
    if(texelBitsSize == 0) {
        mr::Log::LogString("Failed Texture::_CalcMemSize. texelBitsSize is zero.", MR_LOG_LEVEL_WARNING);
        return;
    }

    for(int i = 0; i < mipMapsNum; ++i) {
        TextureSizeInfo mipMapSize = GetSizesInfo(i);
        _memSize += (mipMapSize.CalcMaxVolume() * texelBitsSize) / 8;
    }
}

Texture::~Texture() {
    Destroy();
}

}
