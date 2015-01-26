#include "RenderBufferObject.hpp"
#include "../MachineInfo.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

mr::IRenderBuffer* _MR_BINDED_RENDER_BUFFER_ = nullptr;

namespace mr {

bool RenderBuffer::Create(mr::ITexture::StorageDataFormat const& storageDataFormat, unsigned int const& width, unsigned int const& height, unsigned int const& samples) {
    if(mr::gl::IsOpenGL45()) glCreateRenderbuffers(1, &_handle);
    else glGenRenderbuffers(1, &_handle);

    //TODO check for errors

    if(mr::gl::IsDirectStateAccessSupported()) {
        if(samples == 0) glNamedRenderbufferStorage(_handle, storageDataFormat, width, height);
        else glNamedRenderbufferStorageMultisample(_handle, samples, storageDataFormat, width, height);
    } else {
        auto was = ReBind();
        if(samples == 0) glRenderbufferStorage(GL_RENDERBUFFER, storageDataFormat, width, height);
        else glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, storageDataFormat, width, height);
        if(was) was->Bind();
    }

    return true;
}

void RenderBuffer::Bind() {
    if(_MR_BINDED_RENDER_BUFFER_ == dynamic_cast<mr::IRenderBuffer*>(this)) return;
    glBindRenderbuffer(GL_RENDERBUFFER, _handle);
    _MR_BINDED_RENDER_BUFFER_ = dynamic_cast<mr::IRenderBuffer*>(this);
}

IRenderBuffer* RenderBuffer::ReBind() {
    auto was = _MR_BINDED_RENDER_BUFFER_;
    Bind();
    return was;
}

void RenderBuffer::Destroy() {
    if(_handle == 0) return;
    glDeleteRenderbuffers(1, &_handle);
    _handle = 0;
    OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
}

RenderBuffer::RenderBuffer() {}
RenderBuffer::~RenderBuffer() {}

void RenderBufferUnBind() {
    if(_MR_BINDED_RENDER_BUFFER_ == nullptr) return;
    _MR_BINDED_RENDER_BUFFER_ = nullptr;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

IRenderBuffer* RenderBufferGetBinded() {
    return _MR_BINDED_RENDER_BUFFER_;
}

}
