#include "RenderBufferObject.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

namespace mr {

bool RenderBuffer::Create(mr::ITexture::StorageDataFormat const& storageDataFormat, unsigned int const& width, unsigned int const& height, unsigned int const& samples) {
    unsigned int handle = 0;
    if(mr::gl::IsOpenGL45()) glCreateRenderbuffers(1, &handle);
    else glGenRenderbuffers(1, &handle);

    SetGPUHandle(handle);

    //TODO check for errors

    if(mr::gl::IsOpenGL45()) {
        if(samples == 0) glNamedRenderbufferStorage(handle, storageDataFormat, width, height);
        else glNamedRenderbufferStorageMultisample(handle, samples, storageDataFormat, width, height);
    } else if(mr::gl::IsDirectStateAccessSupported()) {
        if(samples == 0) glNamedRenderbufferStorageEXT(handle, storageDataFormat, width, height);
        else glNamedRenderbufferStorageMultisampleEXT(handle, samples, storageDataFormat, width, height);
    } else {
        Bind();
        if(samples == 0) glRenderbufferStorage(GL_RENDERBUFFER, storageDataFormat, width, height);
        else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, storageDataFormat, width, height);
    }

    return true;
}

void RenderBuffer::Bind() {
    glBindRenderbuffer(GL_RENDERBUFFER, GetGPUHandle());
}

void RenderBuffer::Destroy() {
    unsigned int handle = GetGPUHandle();
    if(handle == 0) return;
    glDeleteRenderbuffers(1, &handle);
    SetGPUHandle(0);
}

RenderBuffer::RenderBuffer() {}
RenderBuffer::~RenderBuffer() {}

}
