#include "FrameBufferObjects.hpp"
#include "FrameBufferConfig.hpp"
#include "../MachineInfo.hpp"
#include "../Utils/Containers.hpp"
#include "RenderBufferInterfaces.hpp"

#define GLEW_STATIC
#include <GL/glew.h>

#define _MR_FB_BIND_TARGETS_NUM_ 3
mr::TStaticArray<mr::IFrameBuffer*> _MR_FB_BIND_TARGETS_(new mr::IFrameBuffer*[_MR_FB_BIND_TARGETS_NUM_], _MR_FB_BIND_TARGETS_NUM_, true);

class _MR_FB_BIND_TARGETS_NULL_ {
public:
    _MR_FB_BIND_TARGETS_NULL_() {
        for(size_t i = 0; i < _MR_FB_BIND_TARGETS_NUM_; ++i){
            _MR_FB_BIND_TARGETS_.GetRaw()[i] = nullptr;
        }
    }
};
_MR_FB_BIND_TARGETS_NULL_ _MR_FB_BIND_TARGETS_NULL_DEF_;

size_t _MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[] {
    GL_DRAW_FRAMEBUFFER,
    GL_READ_FRAMEBUFFER,
    GL_FRAMEBUFFER
};

namespace mr {

bool FrameBuffer::Create() {
    if(GetGPUHandle() != 0) {
        Destroy();
    }

    if(mr::gl::IsOpenGL45())
        glCreateFramebuffers(1, &_handle);
    else
        glGenFramebuffers(1, &_handle);

    return true;
}

void FrameBuffer::Bind(BindTarget const& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return;
    }

    Assert((size_t)target < _MR_FB_BIND_TARGETS_NUM_);
    Assert(GetGPUHandle() != 0);

    if(_MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target] == dynamic_cast<mr::IFrameBuffer*>(this)) return;
    _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target] = dynamic_cast<mr::IFrameBuffer*>(this);
    _bindedTarget = target;

    MR_FRAMEBUFFERS_CHECK_BIND_ERRORS_CATCH(
        glBindFramebuffer(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], GetGPUHandle());
    )
}

IFrameBuffer* FrameBuffer::ReBind(BindTarget const& target) {
    if(target == NotBinded) {
        _bindedTarget = target;
        return 0;
    }

    Assert((size_t)target < _MR_FB_BIND_TARGETS_NUM_);

    IFrameBuffer* binded = _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target];
    Bind(target);
    return binded;
}

IFrameBuffer::CompletionStatus FrameBuffer::CheckCompletion(BindTarget const& target) {
    if(mr::gl::IsDirectStateAccessSupported()) {
        return (CompletionStatus)glCheckNamedFramebufferStatusEXT(GetGPUHandle(), _MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target]);
    }
    else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) {
            binded = mr::FrameBuffer::ReBind(target);
        }
        auto result = glCheckFramebufferStatus(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target]);
        if(binded) binded->Bind(target);
        return (CompletionStatus)result;
    }
}

bool FrameBuffer::SetTextureMipmap(ITexture* tex, Attachment const& attachment, unsigned int const& mipmapLevel) {
    unsigned int texHandle = (tex != nullptr) ? tex->GetGPUHandle() : 0;

    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferTexture(GetGPUHandle(), attachment, texHandle, mipmapLevel);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glFramebufferTexture(IFrameBuffer::DrawReadFramebuffer, attachment, texHandle, mipmapLevel);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::SetTextureMipmapToColor(ITexture* tex, unsigned int const& colorSlot, unsigned int const& mipmapLevel) {
    unsigned int texHandle = (tex != nullptr) ? tex->GetGPUHandle() : 0;

    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferTexture(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorSlot, texHandle, mipmapLevel);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glFramebufferTexture(IFrameBuffer::DrawReadFramebuffer, GL_COLOR_ATTACHMENT0 + colorSlot, texHandle, mipmapLevel);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::SetRenderBuffer(IRenderBuffer* renderBuffer, Attachment const& attachment) {
#warning May be bind render buffer here?

    unsigned int renderBufferHandle = (renderBuffer != nullptr) ? renderBuffer->GetGPUHandle() : 0;

    if(mr::gl::IsOpenGL45()) {
        glNamedFramebufferRenderbuffer(GetGPUHandle(), attachment, GL_RENDERBUFFER, renderBufferHandle);
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferRenderbufferEXT(GetGPUHandle(), attachment, GL_RENDERBUFFER, renderBufferHandle);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        auto bindedRB = renderBuffer->ReBind();
        glFramebufferRenderbuffer(IFrameBuffer::DrawReadFramebuffer, attachment, GL_RENDERBUFFER, renderBufferHandle);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
        if(bindedRB) bindedRB->Bind();
    }
    return true;
}

bool FrameBuffer::SetRenderBufferToColor(IRenderBuffer* renderBuffer, unsigned int const& colorSlot) {
#warning May be bind render buffer here?

    unsigned int renderBufferHandle = (renderBuffer != nullptr) ? renderBuffer->GetGPUHandle() : 0;

    if(mr::gl::IsOpenGL45()) {
        glNamedFramebufferRenderbuffer(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorSlot, GL_RENDERBUFFER, renderBufferHandle);
    }
    else if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferRenderbufferEXT(GetGPUHandle(), GL_COLOR_ATTACHMENT0 + colorSlot, GL_RENDERBUFFER, renderBufferHandle);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        auto bindedRB = renderBuffer->ReBind();
        glFramebufferRenderbuffer(IFrameBuffer::DrawReadFramebuffer, GL_COLOR_ATTACHMENT0 + colorSlot, GL_RENDERBUFFER, renderBufferHandle);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
        if(bindedRB) bindedRB->Bind();
    }
    return true;
}

void FrameBuffer::Destroy() {
    glDeleteFramebuffers(1, &_handle);
    _handle = 0;
    OnGPUHandleChanged(dynamic_cast<mr::IGPUObjectHandle*>(this), 0);
}

bool FrameBuffer::DrawToTarget(TargetBuffer const& targetBuffer) {
    //TODO check for errors
    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferDrawBuffer(_handle, targetBuffer);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glDrawBuffer(targetBuffer);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::DrawToAttachment(Attachment const& attachment) {
    //TODO check for errors
    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferDrawBuffer(_handle, attachment);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glDrawBuffer(attachment);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::DrawToColor(unsigned int const& colorSlot) {
    //TODO check for errors
    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferDrawBuffer(_handle, GL_COLOR_ATTACHMENT0 + colorSlot);
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glDrawBuffer(GL_COLOR_ATTACHMENT0 + colorSlot);
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

bool FrameBuffer::DrawTo(mu::ArrayHandle<TargetBuffer> const& targetBuffers,
                        mu::ArrayHandle<Attachment> const& attachments,
                        mu::ArrayHandle<unsigned int> const& colorSlots)
{
    //TODO check for errors
    mu::ArrayHandle<unsigned int> targets = mu::Combine(mu::CombineCast<unsigned int, TargetBuffer, Attachment>(targetBuffers, attachments, true), colorSlots, true);
    if(mr::gl::IsDirectStateAccessSupported()) {
        glNamedFramebufferDrawBuffers(_handle, targets.GetNum(), targets.GetArray());
    } else {
        IFrameBuffer* binded = nullptr;
        if(_bindedTarget == NotBinded) binded = mr::FrameBuffer::ReBind(IFrameBuffer::DrawReadFramebuffer);
        glDrawBuffers(targets.GetNum(), targets.GetArray());
        if(binded) binded->Bind(IFrameBuffer::DrawReadFramebuffer);
    }
    return true;
}

FrameBuffer::~FrameBuffer() {}

bool DrawTo(IFrameBuffer::TargetBuffer const& targetBuffer) {
    //TODO check for errors
    auto was1 = FrameBufferGetBinded(IFrameBuffer::BindTarget::DrawFramebuffer);
    auto was2 = FrameBufferGetBinded(IFrameBuffer::BindTarget::DrawReadFramebuffer);
    if(was1) FrameBufferUnBind(IFrameBuffer::BindTarget::DrawFramebuffer);
    if(was2) FrameBufferUnBind(IFrameBuffer::BindTarget::DrawReadFramebuffer);
    glDrawBuffer(targetBuffer);
    if(was1) was1->Bind(IFrameBuffer::BindTarget::DrawFramebuffer);
    if(was2) was2->Bind(IFrameBuffer::BindTarget::DrawReadFramebuffer);
    return true;
}

IFrameBuffer* FrameBufferGetBinded(IFrameBuffer::BindTarget const& target) {
    return _MR_FB_BIND_TARGETS_.GetRaw()[(size_t)target];
}

void FrameBufferUnBind(IFrameBuffer::BindTarget const& target) {
    auto ar = _MR_FB_BIND_TARGETS_.GetRaw();
    if(ar[target] == nullptr) return;
    ar[target] = nullptr;
    glBindFramebuffer(_MR_FB_BIND_TARGETS_REMAP_FROM_INDEX_[(size_t)target], 0);
}

}