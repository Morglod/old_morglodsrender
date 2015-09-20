#include "RenderBuffer.hpp"
#include "../Utils/Log.hpp"
#include "../MachineInfo.hpp"
#include "../Textures/Texture.hpp"
#include "../StateCache.hpp"

#include <GL\glew.h>

namespace mr {

bool RenderBuffer::Create(Texture::StorageDataFormat const& sdf,
                        glm::uvec2 const& size,
                        int const& samples /*multisample*/ )
{
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        mr::Log::LogString("Failed RenderBuffer::Create. RenderBuffer already created.", MR_LOG_LEVEL_ERROR);
        return false;
    }

    if(mr::gl::IsOpenGL45()) glCreateRenderbuffers(1, &handle);
    else glGenRenderbuffers(1, &handle);
    SetGPUHandle(handle);

    if(mr::gl::IsDSA_ARB()) {
        if(samples == 0) glNamedRenderbufferStorage(handle, (unsigned int)sdf, size.x, size.y);
        else glNamedRenderbufferStorageMultisample(handle, samples, (unsigned int)sdf, size.x, size.y);
    } else if(mr::gl::IsDSA_EXT()) {
        if(samples == 0) glNamedRenderbufferStorageEXT(handle, (unsigned int)sdf, size.x, size.y);
        else glNamedRenderbufferStorageMultisampleEXT(handle, samples, (unsigned int)sdf, size.x, size.y);
    } else {
        StateCache* stateCache = StateCache::GetDefault();
        RenderBuffer* was = stateCache->GetBindedRenderbuffer();
        if(!stateCache->ReBindRenderbuffer(this, &was)) {
            mr::Log::LogString("Failed RenderBuffer::Create. Failed bind renderbuffer.", MR_LOG_LEVEL_ERROR);
            return false;
        }
        if(samples == 0) glRenderbufferStorage(GL_RENDERBUFFER, (unsigned int)sdf, size.x, size.y);
        else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, (unsigned int)sdf, size.x, size.y);
        if(was) stateCache->BindRenderbuffer(was);
    }

    return true;
}

void RenderBuffer::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle != 0) {
        glDeleteRenderbuffers(1, &handle);
        SetGPUHandle(0);
    }
}

RenderBuffer::~RenderBuffer() {
    Destroy();
}

}
