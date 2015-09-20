#include "Texture2D.hpp"
#include "../StateCache.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"

#include <GL\glew.h>

namespace mr {

bool Texture2D::Create(CreationParams const& params) {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        mr::Log::LogString("Failed Texture2D::Create. Texture already created.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    SetType(Texture::Base2D);
    unsigned int texType = (unsigned int) GetType();

    if(mr::gl::IsOpenGL45()) glCreateTextures(texType, 1, &handle);
    else glGenTextures(1, &handle);
    SetGPUHandle(handle);

    if(mr::gl::IsDSA_ARB()) {
        glTextureParameteri(handle, GL_TEXTURE_WRAP_S, params.wrapS);
        glTextureParameteri(handle, GL_TEXTURE_WRAP_T, params.wrapT);
        //glTextureParameteri(handle, GL_TEXTURE_WRAP_R, params.wrapR);
        glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, params.magFilter);
    }
    else if(mr::gl::IsDSA_EXT()) {
        glTextureParameteriEXT(handle, texType, GL_TEXTURE_WRAP_S, params.wrapS);
        glTextureParameteriEXT(handle, texType, GL_TEXTURE_WRAP_T, params.wrapT);
        //glTextureParameteriEXT(handle, texType, GL_TEXTURE_WRAP_R, params.wrapR);
        glTextureParameteriEXT(handle, texType, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTextureParameteriEXT(handle, texType, GL_TEXTURE_MAG_FILTER, params.magFilter);
    }
    else {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
            mr::Log::LogString("Failed in Texture2D::Create. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(!stateCache->BindTextureNotCached(0, handle, texType)) {
            mr::Log::LogString("Failed in Texture2D::Create. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glTexParameteri(texType, GL_TEXTURE_WRAP_S, params.wrapS);
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, params.wrapT);
        //glTexParameteri(texType, GL_TEXTURE_WRAP_R, params.wrapR);
        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, params.minFilter);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, params.magFilter);

        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }

    return true;
}

bool Texture2D::SetData(TextureDataPtr const& data, Texture::StorageDataFormat const& sdf_) {
    if(data == nullptr) {
        mr::Log::LogString("Failed Texture2D::SetData. data is null.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(!data->IsGood()) {
        mr::Log::LogString("Failed Texture2D::SetData. data is bad.", MR_LOG_LEVEL_ERROR);
        return false;
    }
    if(sdf_ == Texture::SDF_NONE) {
        mr::Log::LogString("Failed Texture2D::SetData. StorageDataFormat is NONE.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    const int mipMapLevel = 0;
    const unsigned int handle = GetGPUHandle();
    const unsigned int texType = GetType();
    const unsigned int sdf = (unsigned int)sdf_;
    const glm::uvec2 size = data->GetSize();
    const unsigned int dataFormat = (unsigned int) data->GetDataFormat();
    const unsigned int dataType =   (unsigned int) data->GetDataType();

    if(mr::gl::IsDSA_ARB()) {
        //glPushClientAttribDefaultEXT(GL_CLIENT_PIXEL_STORE_BIT);
        glTextureImage2DEXT(handle, texType, mipMapLevel, sdf, size.x, size.y, 0, dataFormat, dataType, data->GetData().GetArray());
        //glPopClientAttrib();
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
            mr::Log::LogString("Failed Texture2D::SetData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(!stateCache->BindTextureNotCached(0, handle, texType)) {
            mr::Log::LogString("Failed Texture2D::SetData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glTexImage2D(texType, mipMapLevel, sdf, size.x, size.y, 0, dataFormat, dataType, data->GetData().GetArray());

        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }

    TextureSizeInfo* sizes = new TextureSizeInfo[1];
    sizes[0].width = size.x;
    sizes[0].width = size.y;
    sizes[0].depth = 1;
    _sizes = mu::ArrayHandle<TextureSizeInfo>(sizes, 1, true, false);

    if(OnDataChanged != nullptr) OnDataChanged(static_cast<Texture*>(this));

    return true;
}

bool Texture2D::Storage(glm::uvec2 const& size, Texture::DataType const& dataType_, Texture::DataFormat const& dataFormat_, Texture::StorageDataFormat const& sdf_) {
    const int mipMapLevel = 0;
    const unsigned int handle = GetGPUHandle();
    const unsigned int texType = GetType();
    const unsigned int sdf = (unsigned int)sdf_;
    const unsigned int dataFormat = (unsigned int) dataFormat_;
    const unsigned int dataType =   (unsigned int) dataType_;

    if(mr::gl::IsDSA_ARB()) {
        //glPushClientAttribDefaultEXT(GL_CLIENT_PIXEL_STORE_BIT);
        glTextureImage2DEXT(handle, texType, mipMapLevel, sdf, size.x, size.y, 0, dataFormat, dataType, 0);
        //glPopClientAttrib();
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
            mr::Log::LogString("Failed Texture2D::Storage. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(!stateCache->BindTextureNotCached(0, handle, texType)) {
            mr::Log::LogString("Failed Texture2D::Storage. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glTexImage2D(texType, mipMapLevel, sdf, size.x, size.y, 0, dataFormat, dataType, 0);

        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }

    TextureSizeInfo* sizes = new TextureSizeInfo[1];
    sizes[0].width = size.x;
    sizes[0].height = size.y;
    sizes[0].depth = 1;
    _sizes = mu::ArrayHandle<TextureSizeInfo>(sizes, 1, true, false);

    return true;
}

bool Texture2D::UpdateData(TextureDataPtr const& data, int const& mipMapLevel, glm::ivec2 const& offset) {
    const unsigned int handle = GetGPUHandle();
    if(gl::IsDSA_ARB()) {
        glTextureSubImage2D(handle, mipMapLevel, offset.x, offset.y, data->GetSize().x, data->GetSize().y, data->GetDataFormat(), data->GetDataType(), data->GetData().GetArray());
    } else if(gl::IsDSA_EXT()) {
        glTextureSubImage2DEXT(handle, (unsigned int)GetType(), mipMapLevel, offset.x, offset.y, data->GetSize().x, data->GetSize().y, data->GetDataFormat(), data->GetDataType(), data->GetData().GetArray());
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        unsigned int binded_gpu_handle = 0;
        unsigned int binded_tex_type = 0;

        if(!stateCache->GetBindedTextureNotCached(0, binded_gpu_handle, binded_tex_type)) {
            mr::Log::LogString("Failed Texture2D::UpdateData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(!stateCache->BindTextureNotCached(0, handle, (unsigned int)GetType())) {
            mr::Log::LogString("Failed Texture2D::UpdateData. Failed bind texture.", MR_LOG_LEVEL_ERROR);
            return false;
        }

        glTexSubImage2D((unsigned int)GetType(), mipMapLevel, offset.x, offset.y, data->GetSize().x, data->GetSize().y, data->GetDataFormat(), data->GetDataType(), data->GetData().GetArray());

        if(binded_gpu_handle != 0) stateCache->BindTextureNotCached(0, binded_gpu_handle, binded_tex_type);
    }

    if(OnDataUpdated != nullptr) OnDataUpdated(static_cast<Texture*>(this));

    return true;
}

}
