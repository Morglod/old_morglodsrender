#include "mr/rtt.hpp"
#include "mr/alloc.hpp"
#include "src/mp.hpp"
#include "mr/pre/glew.hpp"
#include "mr/log.hpp"

namespace mr {

FrameBufferPtr FrameBuffer::Create() {
    MP_ScopeSample(FrameBuffer::Create);

    uint32_t handle = 0;
    glCreateFramebuffers(1, &handle);

    auto sh = MR_NEW_SHARED(FrameBuffer);
    sh->_id = handle;

    return sh;
}

void FrameBuffer::Destroy() {
    MP_ScopeSample(FrameBuffer::Destroy);
    if(_id == 0) return;
    glDeleteFramebuffers(1, &_id);
    _id = 0;
}

bool FrameBuffer::SetColor(Texture2DPtr const& texture, const uint16_t attachmentIndex, const int32_t textureTargetMipmapLevel) {
    MP_ScopeSample(FrameBuffer::SetColor);
    const uint32_t textureHandle = (texture != nullptr) ? texture->GetId() : 0;
    glNamedFramebufferTexture(_id, GL_COLOR_ATTACHMENT0 + attachmentIndex, textureHandle, textureTargetMipmapLevel);
    return true;
}

bool FrameBuffer::SetDepth(Texture2DPtr const& texture, const int32_t textureTargetMipmapLevel) {
    MP_ScopeSample(FrameBuffer::SetDepth);
    const uint32_t textureHandle = (texture != nullptr) ? texture->GetId() : 0;
    glNamedFramebufferTexture(_id, GL_DEPTH_ATTACHMENT, textureHandle, textureTargetMipmapLevel);
    return true;
}

bool FrameBuffer::SetStencil(Texture2DPtr const& texture, const int32_t textureTargetMipmapLevel) {
    MP_ScopeSample(FrameBuffer::SetStencil);
    const uint32_t textureHandle = (texture != nullptr) ? texture->GetId() : 0;
    glNamedFramebufferTexture(_id, GL_STENCIL_ATTACHMENT, textureHandle, textureTargetMipmapLevel);
    return true;
}

bool FrameBuffer::SetColor(RenderBufferPtr const& renderbuffer, const uint16_t attachmentIndex) {
    MP_ScopeSample(FrameBuffer::SetColor);
    const uint32_t renderbufferHandle = (renderbuffer != nullptr) ? renderbuffer->GetId() : 0;
    glNamedFramebufferRenderbuffer(_id, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_RENDERBUFFER, renderbufferHandle);
    return true;
}

bool FrameBuffer::SetDepth(RenderBufferPtr const& renderbuffer) {
    MP_ScopeSample(FrameBuffer::SetDepth);
    const uint32_t renderbufferHandle = (renderbuffer != nullptr) ? renderbuffer->GetId() : 0;
    glNamedFramebufferRenderbuffer(_id, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbufferHandle);
    return true;
}

bool FrameBuffer::SetStencil(RenderBufferPtr const& renderbuffer) {
    MP_ScopeSample(FrameBuffer::SetStencil);
    const uint32_t renderbufferHandle = (renderbuffer != nullptr) ? renderbuffer->GetId() : 0;
    glNamedFramebufferRenderbuffer(_id, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbufferHandle);
    return true;
}

bool FrameBuffer::CheckCompletion(FrameBufferCompletion& out_status) {
    MP_ScopeSample(FrameBuffer::CheckCompletion);
    unsigned int result = glCheckNamedFramebufferStatus(_id, GL_FRAMEBUFFER);
    out_status = (FrameBufferCompletion)result;
    return (result == GL_FRAMEBUFFER_COMPLETE);
}

bool FrameBuffer::IsColorAttached(const uint16_t attachmentIndex) {
    MP_ScopeSample(FrameBuffer::IsColorAttached);
    int32_t result = 0;
    glGetNamedFramebufferAttachmentParameteriv(_id, GL_COLOR_ATTACHMENT0 + attachmentIndex, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &result);
    return (result != GL_NONE);
}

bool FrameBuffer::IsDepthAttached() {
    MP_ScopeSample(FrameBuffer::IsDepthAttached);
    int32_t result = 0;
    glGetNamedFramebufferAttachmentParameteriv(_id, GL_DEPTH_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &result);
    return (result != GL_NONE);
}

bool FrameBuffer::IsStencilAttached() {
    MP_ScopeSample(FrameBuffer::IsStencilAttached);
    int32_t result = 0;
    glGetNamedFramebufferAttachmentParameteriv(_id, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &result);
    return (result != GL_NONE);
}

bool FrameBuffer::ToScreen(FrameBufferBit const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear) {
    MP_ScopeSample(FrameBuffer::ToScreen);
    glBlitNamedFramebuffer(_id, 0, src.x, src.y, src.z, src.w, dst.x, dst.y, dst.z, dst.w, (uint32_t)bitflags, (linear) ? GL_LINEAR : GL_NEAREST);
    return true;
}

bool FrameBuffer::ToFrameBuffer(FrameBufferPtr const& other, FrameBufferBit const& bitflags, glm::lowp_uvec4 const& src, glm::lowp_uvec4 const& dst, bool linear) {
    MP_ScopeSample(FrameBuffer::ToFrameBuffer);
    glBlitNamedFramebuffer(_id, other->_id, src.x, src.y, src.z, src.w, dst.x, dst.y, dst.z, dst.w, (uint32_t)bitflags, (linear) ? GL_LINEAR : GL_NEAREST);
    return true;
}

FrameBuffer::FrameBuffer() : _id(0) {
}

FrameBuffer::~FrameBuffer() {
    Destroy();
}

RenderBufferPtr RenderBuffer::Create(RenderBufferParams const& params) {
    MP_ScopeSample(RenderBuffer::Create);

    uint32_t handle = 0;
    glCreateRenderbuffers(1, &handle);

    if(params.samples == 0) glNamedRenderbufferStorage(handle, (uint32_t)params.format, params.size.x, params.size.y);
    else glNamedRenderbufferStorageMultisample(handle, params.samples, (uint32_t)params.format, params.size.x, params.size.y);

    auto rbuffer = MR_NEW_SHARED(RenderBuffer);
    rbuffer->_id = handle;

    return rbuffer;
}

void RenderBuffer::Destroy() {
    MP_ScopeSample(RenderBuffer::Destroy);

    if(_id == 0) return;
    glDeleteRenderbuffers(1, &_id);
    _id = 0;
}

RenderBuffer::RenderBuffer() : _id(0) {}

RenderBuffer::~RenderBuffer() {
    Destroy();
}

MR_API void SetDrawFramebuffer(FrameBufferPtr const& target) {
    MP_ScopeSample(SetDrawFramebuffer);
    uint32_t handle = (target != nullptr) ? target->GetId() : 0;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, handle);
}

MR_API void SetReadFramebuffer(FrameBufferPtr const& target) {
    MP_ScopeSample(SetReadFramebuffer);
    uint32_t handle = (target != nullptr) ? target->GetId() : 0;

    glBindFramebuffer(GL_READ_FRAMEBUFFER, handle);
}

}
